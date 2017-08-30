/*
 * =====================================================================================
 *
 *       Filename:  AlgoMode1Strategy.C
 *
 *    Description:  Strategy Flow:-
 *                    New strategy was spawned in storage/AlertMessageStore.C
 *                    Strategy maintains several states.
 *                    AlgoMode1State_BEGIN ==> Initial state.
 *                    AlgoMode1State_POINT_A ==> After start price crossed point A price.
 *                    AlgoMode1State_POINT_B ==> After Point A price crossed Point B. Once point B reached it checks for the trigger
 *                                               price and if price crossed trigger price strategy send Bracket order and change
 *                                               state to AlgoMode1State_ORDER_PENDING.
 *                    AlgoMode1State_REATTEMPT ==> Primary IOC order canceled by FXCM or Liquidity provider. If conditions are
 *                                                 still favorable will keep on sending Bracket order.
 *                    AlgoMode1State_STOPPED ==> Strategy stopped either timer expired, Point A was true before starting strategy,
 *                                               Max reattempt reached and order still not filled.
 *                    AlgoMode1State_ORDER_PENDING ==> New Bracket order sent to FXCM and waiting for the confirmation.
 *                    AlgoMode1State_REATTEMPT_SECONDARY ==> Once texit reached strategy will try to modify secondary limit order.
 *                    AlgoMode1State_REATTEMPT_SECONDARY_WAITING ==> Secondary order modified and waiting for ACK/NACK from FXCM.
 *                    AlgoMode1State_REATTEMPT_SECONDARY_ACTIVE ==> Maximum reattempt exhausted for secondary order.
 *                  Along with state strategy maintains status :
 *                    StrategyStatus_PENDING ==> Not used.
 *                    StrategyStatus_WAITING ==> If start time is enabled it will wait for it to be true.
 *                    StrategyStatus_STOPPED ==> Strategy Stopped. alertstore.cpp checks this state if stopped it will remove it from
 *                                               running strategy map and add to stopped strategy.
 *                    StrategyStatus_STARTED  ==> Not used.
 *                    StrategyStatus_RUNNING ==> If start time is enabled and current time is greater the user input start time.
 *                                               Otherwise it will come to this state just after start.
 *                    StrategyStatus_UNKNOWN  ==> Not used.
 *
 *                  Known issues with current implementation need to be fixed:-
 *                    1) Strategy secondary order modification reject is not handled in FIX order handler. Fix Op handler does not
 *                       handle modification reject message.
 *
 *        Created:  09/24/2016 12:29:39 PM
 *       Compiler:  g++
 *
 *         Author:  Soumya Prasad Ukil
 *        Company:  AlgoEngine
 *
 * =====================================================================================
 */

#include <chrono>
#include <shared/quotes.h>
#include <shared/iquotesprovider.h>
#include <shared/ialertsstore.h>
#include <shared/iophandler.h>
#include <shared/idatetimeprovider.h>
#include <shared/common.h>
#include <shared/easylogging++.h>
#include <shared/time_util.h>
#include <storage/OrderStore.H>
#include <config/Configuration.H>
#include <strategy/AlgoMode1Strategy.H>
#include <constant/ServerGlobals.H>
#include <constant/AlgoOrderType.H>
#include <constant/AlgoStrategyStatus.H>
#include <symbol/InstrumentMap.H>
#include <constant/ServerGlobals.H>

#include <db/InternalEvent_odb.H>
#include <db/Order_odb.H>
#include <db/RejectEvent.H>
#include <db/Backend.H>

namespace Algo
{
  boost::posix_time::ptime AlgoMode1Strategy::getPtime(std::string date, std::string time)
  {
      namespace bt = boost::posix_time;
      bt::ptime pt;
      std::locale format(std::locale::classic(),new bt::time_input_facet("%m/%d/%Y %H:%M:%S"));
      std::istringstream is(date+" "+time);
      is.imbue(format);
      is >> pt;
      return pt;
  }

  AlgoMode1Strategy::AlgoMode1Strategy()
  {

  }
  AlgoMode1Strategy::AlgoMode1Strategy(PushToServerMsgQueue serverMsgQueue)
    : GenericStrategy(serverMsgQueue) {
    }
  AlgoMode1Strategy::AlgoMode1Strategy(AlgoModel1 &algo, PushToServerMsgQueue serverMsgQueue)
    : GenericStrategy(serverMsgQueue)
  {
    // TODO(ap) Why aren't we using initializers?
    _algoModel1 = algo;
    _dataSymbol.setName(algo.GetDataSymbol());
    _dataSymbol.setSecType(algo.GetDataSecType());
    _dataSymbol.setExchange(algo.GetDataExchange());
    _dataSymbol.setCurrency(algo.GetDataCurrency());
    _dataSymbol.setId(InstrumentMap::getInstance().getSymbolId(_dataSymbol));
    _mode = (OrderMode) algo.GetMode();
    _pointABeforeStart = false;
    _attempts = 0;
    _userName = std::string(algo.GetUserName());

    _lastState = AlgoMode1State_BEGIN;
    _lastKnownPrice = 0;
    _algoType = (AlgoType) algo.GetAlgoType();
    _bPointBReachedBeforePointA = false;

    _afterRestart = false;
    std::map<std::string, int>::iterator iter = openStrategyAllowanceMap.find(_dataSymbol);
    if (iter == openStrategyAllowanceMap.end())
    {
      openStrategyAllowanceMap[_dataSymbol] = 0;
    }
    // TODO(ap) This can be refactored and made easier
    if (_mode == OrderMode_BUY) {
      //cancel pips point A = 1.15221 (parament order limit + cancel pips)
      //cancel pips point B = 1.15129 (trigger - cancel pips)
      _parentOrderPrice = _algoModel1.getParentOrderPrice();
      _triggerPrice = _algoModel1.GetEntryPriceCalc();
      _cancelTickCeil = _parentOrderPrice + (double) _algoModel1.GetCancelTicks() * _algoModel1.GetTickSize();
      _cancelTickFloor = _triggerPrice - (double) _algoModel1.GetCancelTicks() * _algoModel1.GetTickSize();
    }
    else {
      _parentOrderPrice = _algoModel1.getParentOrderPrice();
      _triggerPrice = _algoModel1.GetEntryPriceCalc();
      _cancelTickFloor = _parentOrderPrice - (double) _algoModel1.GetCancelTicks() * _algoModel1.GetTickSize();
      _cancelTickCeil = _triggerPrice + (double) _algoModel1.GetCancelTicks() * _algoModel1.GetTickSize();
    }

    LOG(INFO) << "[STRATEGY] Parent order Price [" << _parentOrderPrice << "] Trigger Price ["
      << _triggerPrice << "] Cancel Tick 1 [" << _cancelTickFloor << "] Cancel Tick 2 ["
      << _cancelTickCeil <<"]";

    if (_algoModel1.GetIsTimeEnabled()) {
      _bisRunning = false;
      _status = StrategyStatus_WAITING;

      _tm = algo.GetStartDateEpoch() + algo.GetStartTimeEpoch();
      _tmStopTime = algo.GetEndDateEpoch() + algo.GetEndTimeEpoch();
    }
    else {
      _bisRunning = true;
      _status = StrategyStatus_RUNNING;
    }

    _clOrderIdFirstLeg = "";
    _clorderIdSecondLegStop = "";
    _clorderIdSecondLegLimit = "";
    _origClorderIdSecondLeg = "";

    _firstLegFilledQty = 0;
    _secondLegFilledQty = 0;

    _firstLegCancelledQty = 0;
    _secondLegCancelledQty = 0;

    _lastBidPrice = 0.0;
    _lastAskPrice = 0.0;
    _lastSavedTime = 0;

    std::string emptyString;
    _algoModel1.SetCancelTime(emptyString.c_str());
    _algoModel1.SetPointATime(emptyString.c_str());
    _algoModel1.SetPointBTime(emptyString.c_str());
    _algoModel1.SetEntryPriceTime(emptyString.c_str());
    _algoModel1.SetLimitTime(emptyString.c_str());
    _algoModel1.SetStopTime(emptyString.c_str());
    _algoModel1.SetStatus(_status);

    initStrategyObject();
    enqueueStrategy(DBOperation::Insert, AlgoStrategyStatus::WAITING);

    char buf[MAX_BUF];
    AlgoModelStrategyId algoModelStrategyId;
    algoModelStrategyId.setStrategyId(_algoModel1.GetStrategyId());
    algoModelStrategyId.setInternalId(_algoModel1.GetInternalId());
    size_t size = algoModelStrategyId.serialize(buf);

    if (_serverMsgQueue) {
      Elements e;
      e._size = size;
      memcpy(e._clientName, _algoModel1.GetUserName(), strlen(_algoModel1.GetUserName()));
      memcpy(e._elements, buf, size);
      _serverMsgQueue(e);
    }
  }

  AlgoMode1Strategy::~AlgoMode1Strategy()
  {
  }

  void AlgoMode1Strategy::processFeed()
  {
    if (_algoModel1.GetIsTimeEnabled() && !_bisRunning) {
      if (!startTimeStatus())
        return;
    }

    if (_algoModel1.GetIsTimeEnabled() && _bisRunning) {
      if (stopTimeStatus())
        return;
    }

    if (!checkFeedUpdate())
      return;

    switch (_lastState) {
      case AlgoMode1State_BEGIN:
        processAlgoBegin();
        break;
      case AlgoMode1State_POINT_A:
        processPointA();
        break;
      case AlgoMode1State_POINT_B:
        processPointB();
        break;
      case AlgoMode1State_ORDER_PENDING:
        {
          // check for T-exit logic here
          if (_algoModel1.getSecondaryOrderExitEnabled()) {
            if ((_firstLegFilledQty + _firstLegCancelledQty) == _algoModel1.GetQty()) {
              if (currentTime() >= (_algoModel1.getSecondaryOrderExitDateEpoch() + _algoModel1.getSecondaryOrderExitTimeEpoch())) {
                LOG(INFO) << "T-exit Activated User input |"
                  << _algoModel1.getSecondaryOrderExitDateEpoch()
                  + _algoModel1.getSecondaryOrderExitTimeEpoch()
                  << "| Current Time |" << currentTime() <<"|";
                _lastState = AlgoMode1State_REATTEMPT_SECONDARY;
              }
            }
          }
        }
        break;
      case AlgoMode1State_REATTEMPT_SECONDARY:
        {
          // send modify of secondary order in 500 milliseconds interval.
          // TODO need to make interval as a configurable value
          long long currentEpochInNillisecond = currentTimeInMilliseconds();
          if (currentEpochInNillisecond > (_lastSavedTime + Configuration::getInstance()->getTExitInterval())) {
            _lastSavedTime = currentEpochInNillisecond;
            modifySecondaryOrder();
          }
        }
        break;
      case AlgoMode1State_REATTEMPT:
        if (!checkCancelTick()) {
          tryNextAttempt();
        }
        break;
      case AlgoMode1State_STOPPED: // Do nothing algo will stop in next iteration.
        break;
      default:
        LOG(ERROR) << "Algo in unknown state. Stopping it";
        _algoModel1.SetStatus(StrategyStatus_STOPPED);
        setCompleted();
        _lastState = AlgoMode1State_STOPPED;
        break;
    }
  }

  void AlgoMode1Strategy::processOrder(const Order &order)
  {
    if (order.getExecType() == 'C') //Canceled
    {
      if (order.getClOrdId() == _clOrderIdFirstLeg
          || order.getClOrdId() == _cancelClOrderIdFirstLeg) {
        // If reattempt is enabled, then only resend primary order.
        if (Configuration::getInstance()->shouldReattemptPrimary()) {
           _lastState = AlgoMode1State_REATTEMPT; // IOC order canceled by matching engine.
        }
        else {
           _lastState = AlgoMode1State_STOPPED;
        }
        _firstLegCancelledQty += order.getCancelledQty();
        LOG(INFO) << "[STRATEGY] Strategy [" << _algoModel1.GetStrategyId() << "] received cancel order for first leg Qty ["
          << order.getCancelledQty() << "]";

        //This has to be picked from tag 52. seems like we are using different timezone
        enqueueIncomingMessageEvent(order.getClOrdId(), order.getExchangeOrderId(),
                                    EventType::Canceled, _triggerPrice,
                                    order.getCancelledQty(), timeFromString(order.getExchangeExitTime()));
        // No need to check further. Primary order cancelled. Needs to reattempt.
        if (Configuration::getInstance()->shouldReattemptPrimary() && 
            _attempts < getMaxAttempts()) {
          _firstLegCancelledQty -= order.getCancelledQty();
          return;
        }
      }
      else if ((order.getClOrdId() == _clorderIdSecondLegLimit)
          || (order.getClOrdId() == _clorderIdSecondLegStop)
          || (order.getClOrdId() == _origClorderIdSecondLeg)
          || _clorderIdsSecondLegLimit.find(order.getClOrdId())
          || _origClorderIdsSecondLegLimit.find(order.getClOrdId())) {
        _lastState = AlgoMode1State_STOPPED;
        _secondLegCancelledQty += order.getCancelledQty();
        LOG(INFO) << "[STRATEGY] Strategy [" << _algoModel1.GetStrategyId() << "] received cancel order for second leg Qty ["
          << order.getCancelledQty() << "]";

        EventType eventType = EventType::Canceled;
        enqueueIncomingMessageEvent(order.getClOrdId(), order.getExchangeOrderId(),
                                    eventType, _triggerPrice,
                                    order.getCancelledQty(), timeFromString(order.getExchangeExitTime()));
      }
      else {
        LOG(ERROR) << "[STRATEGY] Unknown client order id in cancel order for strategy ["
          << _algoModel1.GetStrategyId() << "]";
      }
    }
    else if (order.getExecType() == '5')//Replaced
    {
      //This has to be picked from tag 52. seems like we are using different timezone
      enqueueIncomingMessageEvent(order.getClOrdId(), order.getExchangeOrderId(),
                                  EventType::TExitAck, order.getPrice(),
                                  order.getQuantity(), timeFromString(order.getExchangeExitTime()));
    }
    else if (order.getExecType() == 'Z')//Replaced reject
    {
      LOG(INFO) << "Recived replaced reject message, " << "[ StrategyID " << std::to_string(_algoModel1.GetStrategyId()) << "]"
      << "_OrderID" + order.getExchangeOrderId() << "_ID " << std::to_string(_algoModel1.GetInternalId()) << " " << _algoModel1.GetUpname();
      _clorderIdsSecondLegLimit.deleteData(order.getClOrdId());
      _origClorderIdsSecondLegLimit.deleteData(order.getOrigClOrdId());
      enqueueRejectEvent(order.getClOrdId(), order.getRejectReason(), timeFromString(order.getExchangeExitTime()));
    }
    else if (order.getExecType() == 'F') //Filled
    {
      if (order.getClOrdId() == _clOrderIdFirstLeg) {
        _firstLegFilledQty = order.getFilledQuantity();

        //This has to be picked from tag 52. seems like we are using different timezone
        enqueueIncomingMessageEvent(order.getClOrdId(), order.getExchangeOrderId(),
                                    EventType::Filled, order.getFilledPrice(),
                                    order.getFilledQuantity(), timeFromString(order.getExchangeExitTime()));
        enqueueStrategy(DBOperation::Update, AlgoStrategyStatus::OPEN);
        openStrategyAllowanceMap[_dataSymbol]++;
        _algoModel1.SetStatus(StrategyStatus_OPEN);
      }
      else if ((order.getClOrdId() == _clorderIdSecondLegLimit)
          || (order.getOrigClOrdId() == _clorderIdSecondLegLimit)
          || (order.getClOrdId() == _clorderIdSecondLegStop)
          || (order.getOrigClOrdId() == _clorderIdSecondLegStop)
          || (order.getClOrdId() == _origClorderIdSecondLeg)
          || (order.getOrigClOrdId() == _origClorderIdSecondLeg)
          || _clorderIdsSecondLegLimit.find(order.getClOrdId())
          || _clorderIdsSecondLegLimit.find(order.getOrigClOrdId())
          || _origClorderIdsSecondLegLimit.find(order.getClOrdId())
          || _origClorderIdsSecondLegLimit.find(order.getOrigClOrdId())) {
        std::string secondaryOrderSide = "";
        char buf[MAX_BUF];
        std::string currentTime = getTimeStr();
        if ((order.getClOrdId() == _clorderIdSecondLegLimit)
         || (order.getOrigClOrdId() == _clorderIdSecondLegLimit)
         || _clorderIdsSecondLegLimit.find(order.getClOrdId())
         || _clorderIdsSecondLegLimit.find(order.getOrigClOrdId())
         || _origClorderIdsSecondLegLimit.find(order.getClOrdId())
         || _origClorderIdsSecondLegLimit.find(order.getOrigClOrdId())) {
          _algoModel1.SetLimitTime(currentTime.c_str());
        }
        else {
          _algoModel1.SetStopTime(currentTime.c_str());
        }

        _secondLegFilledQty = order.getFilledQuantity();
        EventType eventType = EventType::Filled;

        //Send message to Front end
        //FILL 12:05:03_EUR/USD _Algo 1_Sell_1000/1000_ID1283123_StatID24392304
        std::string reason = "FILLED: SecondaryOrder_";
        if ((order.getClOrdId() == _clorderIdSecondLegLimit)
         || (order.getOrigClOrdId() == _clorderIdSecondLegLimit)
         || _clorderIdsSecondLegLimit.find(order.getClOrdId())
         || _clorderIdsSecondLegLimit.find(order.getOrigClOrdId())
         || _origClorderIdsSecondLegLimit.find(order.getClOrdId())
         || _origClorderIdsSecondLegLimit.find(order.getOrigClOrdId())) {
          secondaryOrderSide = "LIMIT";
        }
        else {
          secondaryOrderSide = "STOP";
        }
        enqueueIncomingMessageEvent(order.getClOrdId(), order.getExchangeOrderId(),
                                    eventType, order.getFilledPrice(),
                                    order.getFilledQuantity(), timeFromString(order.getExchangeExitTime()));
        _algoModel1.SetStatus(StrategyStatus_STOPPED);
        _lastState = AlgoMode1State_STOPPED; //This will stop t-exit loop.
        openStrategyAllowanceMap[_dataSymbol]--;
      }
      else {
        LOG(ERROR) << "[STRATEGY] Unknown Deal (Fill) message received with Client Order Id ["
          << order.getClOrdId()
          << "] and strategy id [" << _algoModel1.GetStrategyId() << "]"
          << "] Current Order ids are _clOrderIdFirstLeg ["
          << _clOrderIdFirstLeg << "] _clorderIdSecondLegLimit [" << _clorderIdSecondLegLimit
          << "] _clorderIdSecondLegStop [" << _clorderIdSecondLegStop <<"] _origClorderIdSecondLeg ["
          << _origClorderIdSecondLeg <<"]";;
      }
    }
    else if (order.getExecType() == 'A') //Acked
    {
      EventType eventType = EventType::Ack;

      if (order.getClOrdId() == _clOrderIdFirstLeg) {
        _exchangeOrderId = order.getExchangeOrderId();
      }
      else
      {
        if(_firstLegFilledQty > 0) {
          eventType = EventType::Active;
        }
      }
      enqueueIncomingMessageEvent(order.getClOrdId(), order.getExchangeOrderId(),
                                  eventType, order.getAckedPrice(),
                                  order.getAckedQuantity(), timeFromString(order.getExchangeExitTime()));
    }

    else if (order.getExecType() == 'X') //Cancel Rejected...
    {
      enqueueRejectEvent(_clOrderIdFirstLeg, order.getRejectReason(), timeFromString(order.getExchangeExitTime()));
      LOG(INFO) << "[STRATEGY] Cancel Request Rejected.... for strategy [" << _algoModel1.GetStrategyId() << "]";
    }
    else if (order.getExecType() == 'S') //Stopped
    {
      _exchangeOrderId = order.getExchangeOrderId();
    }
    else if (order.getExecType() == 'R') //New Rejected
    {
      LOG(INFO) << "[STRATEGY] Strategy Order Rejected.";
      if (order.getClOrdId() == _clOrderIdFirstLeg) {
        _firstLegCancelledQty += order.getCancelledQty();
        //This has to be picked from tag 52. seems like we are using different timezone
        enqueueRejectEvent(_clOrderIdFirstLeg, order.getRejectReason(), timeFromString(order.getExchangeExitTime()));

        LOG(INFO) << "[STRATEGY] First Leg  rejected Qty ["
          << _firstLegCancelledQty << "] for Strategy ["
          << _algoModel1.GetStrategyId() << "]";
      }
      else if (order.getClOrdId() == _clorderIdSecondLegLimit
          || order.getClOrdId() == _clorderIdSecondLegStop) {
        _secondLegCancelledQty += order.getCancelledQty();
        //This has to be picked from tag 52. seems like we are using different timezone
        enqueueRejectEvent(order.getClOrdId(), order.getRejectReason(), timeFromString(order.getExchangeExitTime()));

        LOG(INFO) << "[STRATEGY] Second Leg  Rejected Qty ["
          << _secondLegCancelledQty << "] for strategy Id ["
          << _algoModel1.GetStrategyId() << "]";
      }
      else {
        LOG(ERROR) << "[STRATEGY] Unknown client order id in Reject order for Strategy ID["
          << _algoModel1.GetStrategyId() << "]";
      }
    }
    if ((_firstLegCancelledQty == _algoModel1.GetQty())
        || (((_firstLegFilledQty + _firstLegCancelledQty) == _algoModel1.GetQty())
          && (_secondLegFilledQty + _secondLegCancelledQty == 2u * _algoModel1.GetQty()))) {
      //Completed
      //Send message to GUI
      _algoModel1.SetStatus(StrategyStatus_STOPPED);

      setCompleted();
    }
  }
  void AlgoMode1Strategy::setAfterRestart()
  {
    _afterRestart = true;
  }
  void AlgoMode1Strategy::sendCompleted()
  {
    _algoModel1.SetStatus(StrategyStatus_STOPPED);
  }

  void AlgoMode1Strategy::setCompleted()
  {
    _algoModel1.SetStatus(StrategyStatus_STOPPED);
    GenericStrategy::setCompleted();
    _origClorderIdsSecondLegLimit.clear();
    _clorderIdsSecondLegLimit.clear();
    enqueueStrategy(DBOperation::Update, AlgoStrategyStatus::STOPPED);

    std::string dataFeedVenueId = Db::Backend::getInstance().dataFeedVenueId();
    IMarketDataHandler *marketDataHandlerPtr = MarketDataHandlerMap::getInstance()
                                               .get(dataFeedVenueId);
    if (!marketDataHandlerPtr->unsubscribe(_dataSymbol)) 
    {
      LOG(ERROR) << "Failed to unsubscribe market data for symbol '" 
                 << _dataSymbol << "'";
    }
  }

  void AlgoMode1Strategy::forceStrategyStop()
  {
    enqueueInternalEvent(EventType::StrategyTerminatedFromGui, 0.0);
    if (_algoModel1.GetStatus() == StrategyStatus_OPEN)
    {
      openStrategyAllowanceMap[_dataSymbol]--;
    }
    setCompleted();
  }

  bool AlgoMode1Strategy::isNotEqual(double a, double b)
  {
    double epsilon = 1e-10;
    return !(std::fabs(a - b) < epsilon);
  }

  bool AlgoMode1Strategy::compareAndCheckEquality(double a, double b, ComparationType comparationType)
  {
    bool result = false;
    if (comparationType == ComparationType::GREATER)
    {
      result = (a > b);
    }
    else if (comparationType == ComparationType::LESS)
    {
      result = (a < b);
    }
    return result || !isNotEqual(a,b);
  }

  bool AlgoMode1Strategy::startTimeStatus()
  {
    time_t now = currentTime();
    if (now > _tm) {
      _status = StrategyStatus_RUNNING;
      _bisRunning = true;
      _algoModel1.SetStatus(_status);
      enqueueInternalEvent(EventType::StrategyChangedPendingToRunning, 0.0);
      enqueueStrategy(DBOperation::Update, AlgoStrategyStatus::RUNNING);
      return true;
    }
    return false;
  }

  bool AlgoMode1Strategy::stopTimeStatus()
  {
    if (_firstLegFilledQty > 0)
      return false; // Strategy should keep running primary order is filled
    time_t now = currentTime();
    if (now > _tmStopTime) {
      _status = StrategyStatus_STOPPED;
      setCompleted();
      _algoModel1.SetStatus(_status);
      enqueueInternalEvent(EventType::StrategyTerminatedDueToStopTime, 0.0);
      return true;
    }
    return false;
  }

  const Db::Instrument& AlgoMode1Strategy::getDataSymbol() const
  {
    return _dataSymbol;
  }

  UNSIGNED_LONG AlgoMode1Strategy::getStrategyId() const
  {
    return _algoModel1.GetStrategyId();
  }

  AlgoMode1State AlgoMode1Strategy::getLastState() const
  {
    return _lastState;
  }

  StrategyStatus AlgoMode1Strategy::getStatus() const
  {
    return (StrategyStatus)_algoModel1.GetStatus();
  }

  time_t AlgoMode1Strategy::currentTime() const
  {
    return TimeUtil::getCurrentTimeEpoch();
  }

  long long AlgoMode1Strategy::currentTimeInMilliseconds() const
  {
    std::chrono::milliseconds ms = std::chrono::duration_cast< std::chrono::milliseconds >(std::chrono::system_clock::now().time_since_epoch());
    return ms.count();
  }

  bool AlgoMode1Strategy::sendOrder() {
    //Send Order
    bool ret = _opHandler && _opHandler->sendBracketOrder(
        _dataSymbol,
        _parentOrderPrice,
        _algoModel1.GetProfitCalc(),
        _algoModel1.GetLossCalc(),
        _algoModel1.GetLossLimitPrice(),
        _triggerPrice,
        _algoModel1.GetQty(),
        _mode,
        _algoModel1.GetTrailingStop(),
        _algoModel1.GetStrategyId(),
        _clOrderIdFirstLeg,
        _clorderIdSecondLegLimit,
        _clorderIdSecondLegStop,
        _algoModel1.GetInternalId(),
        _algoType,
        true,
        _algoModel1.GetUpname(),
        _algoModel1.GetIdentifier());

    if(!ret)
      return false;

    LOG(INFO) << "[STRATEGY] After sendOrder, current bid price: "
      << _priceBid << " ask price: " << _priceAsk;

    // Save order to DB
    OrderMode secondaryOrderMode = _mode == OrderMode_BUY ? OrderMode_SELL
                                                          : OrderMode_BUY;

    enqueueOrder(EventType::BracketOrder, _parentOrderPrice, _algoModel1.GetQty(), AlgoOrderType::Primary, _clOrderIdFirstLeg, _mode);
    enqueueOrder(EventType::BracketOrder, _algoModel1.GetProfitCalc(), _algoModel1.GetQty(), AlgoOrderType::SecondaryLimit, _clorderIdSecondLegLimit, secondaryOrderMode);
    enqueueOrder(EventType::BracketOrder, _algoModel1.GetLossCalc(), _algoModel1.GetQty(), AlgoOrderType::SecondaryStop, _clorderIdSecondLegStop, secondaryOrderMode);
    ++_attempts;
    _clorderIdsSecondLegLimit.addData(_clorderIdSecondLegLimit);
    return true;
  }

  std::string AlgoMode1Strategy::getTimeStr(const std::string& format)
  {
    using namespace boost::posix_time;
    using namespace boost::gregorian;

    // obtain a specific time zone from the database
    ptime now = boost::posix_time::microsec_clock::universal_time();
    boost::local_time::local_date_time nyNow(
        now, Configuration::getInstance()->getTZNewYork());
    struct tm timeinfo = to_tm(nyNow);

    char buffer[80];

    strftime(buffer, 80, "%H:%M:%S", &timeinfo);
    return std::string(buffer);
  }

  void AlgoMode1Strategy::reportCancelTickEvent(
      const std::string& cancelTickType)
  {
    EventType eventType = EventType::CancelTickA;
    if (cancelTickType == "B") {
      eventType = EventType::CancelTickB;
    }
    enqueueInternalEvent(eventType, _algoModel1.GetCancelTicks());
  }

  void AlgoMode1Strategy::processAlgoBegin()
  {
    // Lot of repetitive code.
    bool reached = false;
    if (_algoType == Algo_1) {
      if ((_mode == OrderMode_BUY
            && compareAndCheckEquality(_priceBid, _algoModel1.GetPointACalc(), ComparationType::GREATER)
            && compareAndCheckEquality(_priceAsk, _algoModel1.GetPointACalc(), ComparationType::GREATER)
          )
          ||
          (_mode == OrderMode_SELL
           && compareAndCheckEquality(_priceBid, _algoModel1.GetPointACalc(), ComparationType::LESS)
           && compareAndCheckEquality(_priceAsk, _algoModel1.GetPointACalc(), ComparationType::LESS))) {
        reached = true;
      }
    }
    else {
      //Algo_2 special case, Check for Point B before point A.
      if ((_mode == OrderMode_BUY
            && compareAndCheckEquality(_priceBid, _algoModel1.GetPointBCalc(), ComparationType::GREATER)
            && compareAndCheckEquality(_priceAsk, _algoModel1.GetPointBCalc(), ComparationType::GREATER))
          ||
          (_mode == OrderMode_SELL
           && compareAndCheckEquality(_priceBid, _algoModel1.GetPointBCalc(), ComparationType::LESS)
           && compareAndCheckEquality(_priceAsk, _algoModel1.GetPointBCalc(), ComparationType::LESS))) {
        _bPointBReachedBeforePointA = true;
        LOG(TRACE) << "[STRATEGY] Point B reached before Point A for Strategy [" << _algoModel1.GetStrategyId() <<"]";
      }

      if (
          (_mode == OrderMode_BUY
           && compareAndCheckEquality(_priceBid, _algoModel1.GetPointACalc(), ComparationType::LESS)
           && compareAndCheckEquality(_priceAsk, _algoModel1.GetPointACalc(), ComparationType::LESS)
          )
          || (_mode == OrderMode_SELL
            && compareAndCheckEquality(_priceBid, _algoModel1.GetPointACalc(), ComparationType::GREATER)
            && compareAndCheckEquality(_priceAsk, _algoModel1.GetPointACalc(), ComparationType::GREATER))) {
        reached = true;
      }
    }

    if (reached) {
      LOG(INFO) << "[STRATEGY]  " << _algoModel1.GetStrategyId() << " Point A reached.";
      if (!_pointABeforeStart) {
        setCompleted();
        enqueueInternalEvent(EventType::PointABeforeStart, _algoModel1.GetPointACalc());

        LOG(INFO) << "[STRATEGY] Point A was true before start [" << _algoModel1.GetStrategyId() << "]";
        return;
      }
      _algoModel1.SetPointATime(getTimeStr().c_str());

      _lastState = AlgoMode1State_POINT_A;
      enqueueInternalEvent(EventType::PointA, _algoModel1.GetPointACalc());
      return;
    }
    _pointABeforeStart = true;
  }

  bool AlgoMode1Strategy::checkForCancelPoint1()
  {
    bool reached = false;
    if (_mode == OrderMode_BUY) {
      if (_algoType == Algo_1) {
        if (compareAndCheckEquality(_lastKnownPrice, _algoModel1.GetCancelPoint1Calc(), ComparationType::GREATER)) {
          reached = true;
        }
      }
      else {
        if (compareAndCheckEquality(_lastKnownPrice, _algoModel1.GetCancelPoint1Calc(), ComparationType::LESS)) {
          reached = true;
        }
      }
    }
    else {
      if (_algoType == Algo_1) {
        if (compareAndCheckEquality(_lastKnownPrice, _algoModel1.GetCancelPoint1Calc(), ComparationType::LESS)) {
          reached = true;
        }
      }
      else {
        if (compareAndCheckEquality(_lastKnownPrice, _algoModel1.GetCancelPoint1Calc(), ComparationType::GREATER)) {
          reached = true;
        }
      }
    }
    if (reached) {
      _algoModel1.SetCPATime(getTimeStr().c_str());

      enqueueInternalEvent(EventType::CancelPoint1, _algoModel1.GetCancelPoint1Calc());

      std::string reason = "CP1_";
      reason +=  _dataSymbol.name() + "_" + AlgoTypeStr[_algoType]
      + "_" + std::to_string(_algoModel1.GetQty()) + "_ID"
      + std::to_string(_algoModel1.GetInternalId()) + "_StatID" + std::to_string(_algoModel1.GetStrategyId()) + "_";
      reason += _algoModel1.GetUpname();
      setCompleted();
      LOG(INFO) << reason;
    }
    return reached;
  }

  /**
   * For Algo_1 Cancel Point 2 will be active only if Point A and Point B both reached,
   *      if algo is of buy side price goes below cancel point 2
   *      reverse for sell side algo.
   *
   * For Algo_2 cancel point 2 will be in picture only if point B becomes true before point A.
   */
  bool AlgoMode1Strategy::checkForCancelPoint2()
  {
    bool reached = false;
    if (_algoType == Algo_2)
    {
      if(!_bPointBReachedBeforePointA)
        return false;
    }
    if (
        (_mode == OrderMode_BUY
         && compareAndCheckEquality(_lastKnownPrice, _algoModel1.GetCancelPoint2Calc(), ComparationType::LESS)
        )
        ||
        (_mode == OrderMode_SELL
         && compareAndCheckEquality(_lastKnownPrice, _algoModel1.GetCancelPoint2Calc(), ComparationType::GREATER))
       ) {
      reached = true;
    }
    if(reached)
    {
      _algoModel1.SetCPBTime(getTimeStr().c_str());

      enqueueInternalEvent(EventType::CancelPoint2, _algoModel1.GetCancelPoint2Calc());

      LOG(INFO) << "[STRATEGY] Cancel Point 2 reached for strategy [" << _algoModel1.GetStrategyId() << "]";
      setCompleted();
      return true;
    }
    return false;
  }

  void AlgoMode1Strategy::processPointA() {
    bool reached = false;
    if (checkForCancelPoint1()) {
      _lastState = AlgoMode1State_STOPPED;
      return;
    }
    if (_algoType == Algo_1) {
      if (
          (_mode == OrderMode_BUY
           &&
           (compareAndCheckEquality(_priceBid, _algoModel1.GetPointBCalc(), ComparationType::LESS)
            && compareAndCheckEquality(_priceAsk, _algoModel1.GetPointBCalc(), ComparationType::LESS))
          )
          ||
          (_mode == OrderMode_SELL
           &&
           (compareAndCheckEquality(_priceBid, _algoModel1.GetPointBCalc(), ComparationType::GREATER)
            && compareAndCheckEquality(_priceAsk, _algoModel1.GetPointBCalc(), ComparationType::GREATER)))
         ) {
        reached = true;
      }
    }
    else {
      if (
          (_mode == OrderMode_BUY
           &&
           (compareAndCheckEquality(_priceBid, _algoModel1.GetPointBCalc(), ComparationType::GREATER)
            && compareAndCheckEquality(_priceAsk, _algoModel1.GetPointBCalc(), ComparationType::GREATER)))
          ||
          (_mode == OrderMode_SELL
           &&
           (compareAndCheckEquality(_priceBid, _algoModel1.GetPointBCalc(), ComparationType::LESS)
            && compareAndCheckEquality(_priceAsk, _algoModel1.GetPointBCalc(), ComparationType::LESS)))
         ) {
        reached = true;
      }
    }
    if (reached) {
      LOG(INFO) << "[STRATEGY] " << _algoModel1.GetStrategyId() << " Point B reached.";
      _algoModel1.SetPointBTime(getTimeStr().c_str());
      _lastState = AlgoMode1State_POINT_B;
      enqueueInternalEvent(EventType::PointB, _algoModel1.GetPointBCalc());
    }
  }

  bool AlgoMode1Strategy::checkForTriggerPrice()
  {
    //TODO: Confirm from doc.. condition is same for both algo 1 and algo 2
    if ((_mode == OrderMode_BUY && (compareAndCheckEquality(_priceBid, _triggerPrice, ComparationType::GREATER) 
                                && compareAndCheckEquality(_priceAsk, _triggerPrice, ComparationType::GREATER)))
        || (_mode == OrderMode_SELL && (compareAndCheckEquality(_priceBid, _triggerPrice, ComparationType::LESS) 
                                    && compareAndCheckEquality(_priceAsk, _triggerPrice, ComparationType::LESS)))) {
      LOG(INFO) << "Last Price [" << _lastKnownPrice << "] _triggerPrice ["
        << _triggerPrice << "] Trigger Price Touched for Strategy ["
        << _algoModel1.GetStrategyId() << "] with internal id ["
        << _algoModel1.GetInternalId() << "] Sending IAC order...";

      _algoModel1.SetEntryPriceTime(getTimeStr().c_str());

      enqueueInternalEvent(EventType::PointC, _triggerPrice);
      int maxOpenStrategyAllowance = Configuration::getInstance()->getOpenStrategyAllowance();

      if ((maxOpenStrategyAllowance != -1) && (openStrategyAllowanceMap[_dataSymbol] >= maxOpenStrategyAllowance))
      {
        LOG(INFO) << "[STRATEGY] Stopping the strategy ["
                  << _algoModel1.GetStrategyId() << "] with internal id ["
                  << _algoModel1.GetInternalId() << "] since current open "
                  << "positions reached to maximum ["
                  << openStrategyAllowanceMap[_dataSymbol]
                  << "] for symbol "
                  << _dataSymbol;
        enqueueInternalEvent(EventType::KilledMaxOpen, _triggerPrice);
        setCompleted();
        _lastState = AlgoMode1State_STOPPED;
        return false;
      }
      if(!sendOrder())
      {
        enqueueInternalEvent(EventType::ErrorSendingIOC, _triggerPrice);
        LOG(ERROR) << "[STRATEGY] Error sending orders stopping the strategy";
        setCompleted();
        _lastState = AlgoMode1State_STOPPED;
        return false;
      }
      return true;
    }
    return false;
  }

  void AlgoMode1Strategy::processPointB()
  {
    if (checkForCancelPoint2()) {
      _lastState = AlgoMode1State_STOPPED;
      return;
    }
    if (checkForTriggerPrice()) {
      _lastState = AlgoMode1State_ORDER_PENDING;
    }
  }

  bool AlgoMode1Strategy::checkCancelTick()
  {
    bool breached = false;

    if (_mode == OrderMode_BUY && _lastKnownPrice > _cancelTickCeil) {
      LOG(INFO) << "Last Price [" << _lastKnownPrice << "] reached above Cancel Tick ["
        << _cancelTickCeil << "] Cancel Tick A ReachedStrategy ["
        << _algoModel1.GetStrategyId() << "] with internal id [" << _algoModel1.GetInternalId() << "]";
      breached = true;
    }
    else if (_mode == OrderMode_SELL && _lastKnownPrice < _cancelTickFloor) {
      LOG(INFO) << "Last Price [" << _lastKnownPrice << "] goes below  Cancel Tick ["
        << _cancelTickFloor << "] Cancel Tick A ReachedStrategy ["
        << _algoModel1.GetStrategyId() << "] with internal id [" << _algoModel1.GetInternalId() << "]";
      breached = true;
    }
    if (breached) {
      reportCancelTickEvent("A");
      _lastState = AlgoMode1State_STOPPED;
      return true;
    }

    if (_mode == OrderMode_BUY && _lastKnownPrice < _cancelTickFloor) {
      LOG(INFO) << "Last Price [" << _lastKnownPrice << "] goes below Cancel Ticks ["
        << _cancelTickFloor << "] Cancel Tick A ReachedStrategy ["
        << _algoModel1.GetStrategyId() << "] with internal id [" << _algoModel1.GetInternalId() << "]";
      breached = true;
    }
    else if (_mode == OrderMode_SELL && _lastKnownPrice > _cancelTickCeil) {
      LOG(INFO) << "Last Price [" << _lastKnownPrice << "] reached above Cancel Tick ["
        << _cancelTickCeil << "] Cancel Tick A ReachedStrategy ["
        << _algoModel1.GetStrategyId() << "] with internal id [" << _algoModel1.GetInternalId() << "]";
      breached = true;
    }
    if (breached) {
      reportCancelTickEvent("B");
      _lastState = AlgoMode1State_STOPPED;
      return true;
    }
    return false;
  }

  void AlgoMode1Strategy::tryNextAttempt()
  {
    if (_attempts >= getMaxAttempts()) {
      _algoModel1.SetStatus(StrategyStatus_STOPPED);
      setCompleted();
      enqueueInternalEvent(EventType::MaxReattemptReached, _triggerPrice);
      _lastState = AlgoMode1State_STOPPED;
    }
    else {
      if(sendOrder()) {
        LOG(INFO) << "Order Sent for reattempt [" << _attempts << "]";
        _lastState = AlgoMode1State_ORDER_PENDING;
      }
      else {
        LOG(ERROR) << "[STRATEGY] Error in sending orders stopping the strategy";
        //Send message to Front end
        enqueueInternalEvent(EventType::ErrorSendingIOC, _triggerPrice);
        setCompleted();
        _lastState = AlgoMode1State_STOPPED;
      }
    }
  }

  bool AlgoMode1Strategy::checkFeedUpdate()
  {
    if (_quotesProvider == nullptr)
      return false;

    Algo::Quotes quote = _quotesProvider->get(_dataSymbol);

    if ((quote.getBidPrice() == 0) || (quote.getAskPrice() == 0)) {
      return false;
    }

    // While loading old running strategies last price returns 0 due to no feed update.
    // so strategy should ignore 0 price feed.
    if(_afterRestart){
      if(!isNotEqual(0, quote.getBidPrice()) || !isNotEqual(0, quote.getAskPrice())){
          return false;
      }
      // I think the following else section is a bug
      // if _afterRestart is false already, why set it again?
      // The else section should really belong to the "return false" section above
    }

    if ((_lastState == AlgoMode1State_ORDER_PENDING) || (_lastState == AlgoMode1State_REATTEMPT_SECONDARY)) {
      return true;
    }

    // TODO(ap) refactoring needed
    if (_mode == OrderMode_BUY) {
      _priceBid = quote.getBidPrice(); // More aggressive should be Ask
      _priceAsk = quote.getAskPrice(); // More aggressive should be Ask
      _lastKnownPrice = quote.getAskPrice();
    }
    else {
      _priceAsk = quote.getAskPrice();
      _priceBid = quote.getBidPrice();
      _lastKnownPrice = quote.getBidPrice();
    }

    if (isNotEqual(_priceBid, _lastBidPrice) || isNotEqual(_priceAsk, _lastAskPrice)) {
      LOG(TRACE) << "[STRATEGY] Current Price Bid [" << _priceBid
        << "] Ask [" << _priceAsk
        << "] Point A [" << _algoModel1.GetPointACalc()
        << "] Point B [" << _algoModel1.GetPointBCalc()
        << "] for strategy [" << _algoModel1.GetStrategyId()
        << "] with internal id [" << _algoModel1.GetInternalId()
        << "] last known price [" << _lastAskPrice
        << "] [" << _lastBidPrice << "]";
      _lastAskPrice = _priceAsk;
      _lastBidPrice = _priceBid;
      return true;
    }
    return false;
  }

  void AlgoMode1Strategy::modifySecondaryOrder() {
    double price = 0.0;
    if (sendModifyOrder(price)) {
      LOG(TRACE) << "T-Exit order successfully sent for strategy [" << _algoModel1.GetStrategyId() << "] with internal id [" << _algoModel1.GetInternalId() << "]";
    }
    else {
      enqueueInternalEvent(EventType::ErrorSendingSecondaryLimit, price);
    }
  }

  bool AlgoMode1Strategy::sendModifyOrder(double &price) {
    std::string orderId;
    Algo::Quotes quote = _quotesProvider->get(_dataSymbol);

    if (_mode == OrderMode_BUY) {
      price = _opHandler->modifiedSecondaryLimitPrice(
        quote.getAskPrice(), _algoModel1.GetTickSize());  // the ticksize adjustment is needed in FXCM, 
                                                          // but not in IB (Futures) +_algoModel1.GetTickSize();
    } else {
      price = _opHandler->modifiedSecondaryLimitPrice(
        quote.getBidPrice(), -_algoModel1.GetTickSize()); // the ticksize adjustment is needed in FXCM,
                                                          //but not in IB (Futures) -_algoModel1.GetTickSize();
    }
    LOG(INFO) << _algoModel1.GetStrategyId() << " Modifying secondary order."
      << " Epoch date time " << _algoModel1.getSecondaryOrderExitDateEpoch() <<":"
      << _algoModel1.getSecondaryOrderExitTimeEpoch();

    if(_clorderIdsSecondLegLimit.size() == 0)
    {
        _clorderIdsSecondLegLimit.addData(_clorderIdSecondLegLimit);
    }
    //send order now
    std::string lastOrderId = _clorderIdsSecondLegLimit.last();
    bool ret = _opHandler &&
      _opHandler->modifyOrder(
          lastOrderId,
          orderId,
          price,
          _algoModel1.GetQty()-_secondLegFilledQty,
          _algoModel1.GetInternalId());
    if (!ret)
      return false;

    OrderMode secondaryOrderMode = OrderMode_BUY;
    if (_mode == OrderMode_BUY) {
       secondaryOrderMode = OrderMode_SELL;
    }
    Algo::Order *orderObj = OrderStore::getInstance().getOrderFromClOrderIdToOrder(lastOrderId);
    enqueueOrder(EventType::TExit, price, _algoModel1.GetQty() - _secondLegFilledQty, 
                 AlgoOrderType::SecondaryLimit, orderObj->getSecondaryOrderId(), secondaryOrderMode);

    _origClorderIdsSecondLegLimit.addData(lastOrderId);
    _origClorderIdSecondLeg = _clorderIdSecondLegLimit;
    _clorderIdSecondLegLimit = orderId;
    _clorderIdsSecondLegLimit.addData(orderId);
    LOG(INFO) << _algoModel1.GetStrategyId() << " Secondary Order original order id ["
      << _origClorderIdSecondLeg << "] new order id [" << _clorderIdSecondLegLimit
      <<"]";
    return true;
  }

  void AlgoMode1Strategy::enqueueIncomingMessageEvent(const std::string &clientOrderId, const std::string &exchangeOrderId, EventType eventType,
                                                      double recvPrice, double recvQuantity, boost::posix_time::ptime timestamp)
  {
    std::shared_ptr<Db::IncomingMessageEvent> event(new Db::IncomingMessageEvent(clientOrderId, Db::Backend::getInstance().id(), 
                                                    exchangeOrderId, eventType, recvQuantity, recvPrice));
    event->setTimestamp(timestamp);
    gIncomingMessageEventsQueue.push(std::make_pair(DBOperation::Insert, event));
  }

  void AlgoMode1Strategy::enqueueRejectEvent(const std::string &clientOrderId, const std::string &reason, boost::posix_time::ptime timestamp)
  {
    std::shared_ptr<Db::RejectEvent> event(new Db::RejectEvent(clientOrderId, Db::Backend::getInstance().id(), reason));

    event->setTimestamp(timestamp);
    gRejectEventsQueue.push(std::make_pair(DBOperation::Insert, event));
  }

  void AlgoMode1Strategy::enqueueOrder(EventType eventType, double sentPrice,
                                       double sentQuantity, AlgoOrderType orderType,
                                       const std::string &clientOrderId, OrderMode side)
  {
    std::shared_ptr<Db::Order> order(new Db::Order(clientOrderId, _strategy->strategyId(), Db::Backend::getInstance().id(), eventType, 
                                     orderType, sentQuantity, sentPrice, side));
    gOrdersQueue.push(std::make_pair(DBOperation::Insert, order));
  }

  void AlgoMode1Strategy::enqueueInternalEvent(EventType eventType, double eventPrice)
  {
    std::shared_ptr<Db::InternalEvent> event(new Db::InternalEvent(_strategy, eventType, eventPrice));
    gInternalEventsQueue.push(std::make_pair(DBOperation::Insert, event));
  }

  void AlgoMode1Strategy::initStrategyObject()
  {
    _strategy = std::make_shared<Db::Strategy>();
    _strategy->setStrategyId(_algoModel1.GetStrategyId());
    _strategy->setBackendId(Db::Backend::getInstance().id());
    _strategy->setInternalId(_algoModel1.GetInternalId());
    _strategy->setUploadId(_algoModel1.GetIdentifier());
    _strategy->setInstrumentId(InstrumentMap::getInstance().getSymbolId(_dataSymbol));
    _strategy->setTickSize(_algoModel1.GetTickSize());
    _strategy->setAlgoType(_algoType);
    _strategy->setMode(_mode);
    _strategy->setPreviousClose(_algoModel1.GetPrevClose());
    _strategy->setBaseValue(_algoModel1.GetBaseValue());
    _strategy->setPointAPrice(_algoModel1.GetPointACalc());
    _strategy->setPointAPercentage(_algoModel1.GetPointA());
    _strategy->setPointBPrice(_algoModel1.GetPointBCalc());
    _strategy->setPointBPercentage(_algoModel1.GetPointB());
    _strategy->setPointCPrice(_algoModel1.GetEntryPriceCalc());
    _strategy->setPointCPercentage(_algoModel1.GetEntryPrice());
    _strategy->setCP1Price(_algoModel1.GetCancelPoint1Calc());
    _strategy->setCP1Percentage(_algoModel1.GetCancelPoint1());
    _strategy->setCP2Price(_algoModel1.GetCancelPoint2Calc());
    _strategy->setCP2Percentage(_algoModel1.GetCancelPoint2());
    _strategy->setParentOrderPrice(_algoModel1.getParentOrderPrice());
    _strategy->setProfit(_algoModel1.GetProfitCalc());
    _strategy->setProfitPercentage(_algoModel1.GetProfit());
    _strategy->setLoss(_algoModel1.GetLossCalc());
    _strategy->setLossPercentage(_algoModel1.GetLoss());
    _strategy->setLossLimitPrice(_algoModel1.GetLossLimitPrice());

    boost::posix_time::ptime t = getPtime(_algoModel1.GetStartDate(), _algoModel1.GetStartTime());
    _strategy->setStartDate(t.date());
    _strategy->setStartTime(t.time_of_day());

    t = getPtime(_algoModel1.GetEndDate(), _algoModel1.GetEndTime());
    _strategy->setEndDate(t.date());
    _strategy->setEndTime(t.time_of_day());

    t = getPtime(_algoModel1.getSecondaryOrderExitDate(), _algoModel1.getSecondaryOrderExitTime());
    _strategy->setTExitDate(t.date());
    _strategy->setTExitTime(t.time_of_day());

    _strategy->setTrailing(_algoModel1.GetTrailingStop());
    _strategy->setTrailingPercentage(_algoModel1.getTrailingPercentage());
    _strategy->setTolerance(_algoModel1.GetTolerance());
    _strategy->setQuantity(_algoModel1.GetQty());
    _strategy->setCancelTicks(_algoModel1.GetCancelTicks());
    _strategy->setUploadName(_algoModel1.GetUpname());
  }

  void AlgoMode1Strategy::enqueueStrategy(DBOperation operation, AlgoStrategyStatus strategyStatus)
  {
    _strategy->setStrategyStatus(strategyStatus);
    gStrategiesQueue.push(std::make_pair(operation, _strategy));
  }
}
