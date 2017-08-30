/*
 * =====================================================================================
 *
 *       Filename:  AlgoMode1Strategy.H
 *
 *    Description:
 *
 *        Created:  09/19/2016 12:11:53 PM
 *       Compiler:  g++
 *
 *         Author:  Soumya Prasad Ukil
 *        Company:  AlgoEngine
 *
 * =====================================================================================
 */

#ifndef AlgoMode1Strategy_H
#define AlgoMode1Strategy_H

#include <strategy/AlgoMode1State.H>
#include <shared/commands.h>
#include <shared/genericstrategy.h>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <shared/order.h>
#include <boost/serialization/access.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/array.hpp>
#include <util/ThreadSafeVector.H>
#include <shared/EventType.H>
#include <constant/DBOperation.H>
#include <constant/ServerGlobals.H>
#include <shared/imarket_data_handler.h>
#include <db/Strategy.H>
#include <db/Instrument.H>

namespace Algo
{

  /*! \class    AlgoMode1Strategy
   *  \brief    Implements Strategy for AlgoMode1
   *
   *  \see Algo::AlgoModel1
   *  \see https://github.com/rslichte/AlgoEngine/wiki/AlgoEngine-main-logic
   */
  class AlgoMode1Strategy : public GenericStrategy
  {
    friend class boost::serialization::access;
    public:
      AlgoMode1Strategy();
      AlgoMode1Strategy(PushToServerMsgQueue serverMsgQueue);

      AlgoMode1Strategy(AlgoModel1 &algo,
                        PushToServerMsgQueue serverMsgQueue = nullptr);
      virtual ~AlgoMode1Strategy();

      void processFeed() override;
      void processOrder(const Order& order) override;
      void sendCompleted() override;
      void setCompleted() override;
      void forceStrategyStop() override;

      /*!
       * \brief Simple boolean comparison helper
       * \return True if a != b, otherwise false
       */
      bool isNotEqual(double a, double b);

      bool compareAndCheckEquality(double a, double b, ComparationType comparationType);

      /*!
       * \brief Starts strategy If current time is greater than predefined time
       *        for startegy start.
       * \return True if started strategy, false if strategy start time not
       *         reached yet.
       */
      bool startTimeStatus();

      /*!
       * \brief Stops strategy If current time is greater than predefined time
       *        for startegy stop and if strategy didn't pass PA, PB, PC.
       * \return True if stopped strategy, false if strategy stop time not
       *         reached yet or strategy already has fulfilled Entry order
       */
      bool stopTimeStatus();

      const Db::Instrument& getDataSymbol() const;
      UNSIGNED_LONG getStrategyId() const;
      AlgoMode1State getLastState() const;
      StrategyStatus getStatus() const;

      /*!
       * \brief Get current UTC time.
       * \return utc time value.
       */
      virtual time_t currentTime() const;
      long long currentTimeInMilliseconds() const;

      /*!
       * \brief Mark strategy that it is restored after program restart. This is
       *        required to handle specific logic after interrupted session.
       */
      virtual void setAfterRestart();

    private:
      boost::posix_time::ptime getPtime(std::string date, std::string time);

      /*!
       * \brief Send bracket order and save corresponding internal events to DB.
       */
      virtual bool sendOrder();

      /*!
       * \brief Get time string in format "%H:%M:%S" in NY timezone.
       *
       * \param format String representation of format in which to return time.
       *               (see for available formats.
       *                http://www.cplusplus.com/reference/ctime/strftime/)
       * \return String with time in given format.
       */
      std::string getTimeStr(const std::string& format = "%H:%M:%S");

      /*!
       * \brief Stores into DB cancel tick event for given cancel tick type,
       *
       * \param cancelTickType Our own name for even
       *                       (e.g. "A" - for point A, "B" -for point B)
       */
      void reportCancelTickEvent(const std::string& cancelTickType);

      /*!
       * \brief Check if point A reached or not. If it reached before getting
       *        first tick update or before algo begin, stop the strategy.
       */
      void processAlgoBegin();

      /*!
       * \brief Check whether Cancel Point 1 rechaed.
       * \return True if reached, othewise false.
       */
      bool checkForCancelPoint1();

      /*!
       * \brief Check whether Cancel Point 1 rechaed.
       * \return True if reached, othewise false.
       */
      bool checkForCancelPoint2();

      /*!
       * \brief Once Point A reached, there are two possible following states
       *        either Cancel Point 1 or Point B. This function checks whether
       *        conditions for these states are met.
       */
      void processPointA();

      /*!
       * \brief Checks whether trigger price of strategy is higher then current
       *        ask and bid prices and sends order to the market.
       * \return True if order was sent, otherwise false.
       */
      bool checkForTriggerPrice();

      /*!
       * \brief Once Point B reached, there are two possible following states
       *        either Cancel Point 2 or Sending Bracket order. This function checks
       *        whether conditions for these states are met.
       */
      void processPointB();

      /*!
       * \brief Checks whether last known price goes beyond cancel tick velue
       *        (ceil or floor value). If last known price is beyond the range,
       *        then stops the strategy.
       * \return True if last known price beyond cancel tick value and strategy
       *         stopped, otherwise returns false.
       */
      bool checkCancelTick();

      /*!
       * \brief If our previous attempt to send order was canceled you can call
       *        this function which will check how many attempts already done
       *        and if it isn't not more than set max attemts.
       *        (\see setMaxAttempts), then it will try to send order again.
       */
      void tryNextAttempt();

      /*!
       * \brief Check whether market ask and bid prices for instrument have
       *        changed from last check.
       * \return True if prices were changed from last check, otherwise false.
       */
      bool checkFeedUpdate();

      /*!
       * \brief Redundant wrapper around \p sendModifyOrder()
       *        TODO: move all it's logic inside sendModifyOrder
       */
      void modifySecondaryOrder();

      /*!
       * \brief Tries to exit the market by sending modification of secondary
       *        limit order. It is usefull for T-Exit functionality, when we
       *        want to exit right now.
       *
       * It uses following price to exit
       * Ask Price + tick size (for BUY strategy)
       * Bid Price - tick size (for SELL strategy).
       * Why needed +- tick size? Because while our order travelling to the
       * market the ask/bid price may change and our order will be rejected
       * because it doesn't match current lowest ask price or highest bid price.
       *
       * \see https://github.com/rslichte/AlgoEngine/wiki/T-Exit-Logic to
       *      understand more about T-Exit
       */
      bool sendModifyOrder(double &price);

      //
      // Db messages
      //

      /*!
       * \brief Adds event into queue of internal events from which it will be
       *        saved into DB
       * \param eventType Type of event.
       * \param eventPrice Event price.
       */
      void enqueueInternalEvent(EventType eventType, double eventPrice);

      /*!
       * \brief Initialize strategy member
       */
      void initStrategyObject();

      /*!
       * \brief Sets new status \p strategyStatus to the current strategy and
       *        and sends itself into the queue of db operations for
       *        strategy table.
       *
       * \param operation DB operation (insert, update, delete).
       * \param strategyStatus new status of strategy.
       */
      void enqueueStrategy(DBOperation operation,
                           AlgoStrategyStatus strategyStatus);

      /*!
       * \brief Adds order with given parameters to the queue of orders to be
       *        saved into DB.
       *
       * \param eventType Type of event on which we send an order.
       * \param sentPrice Order price that we send.
       * \param sentQuantity Order quantity that we send.
       * \param orderType Order type.
       * \param clientOrderId Client order id.
       * \param side Buy or Sell.
       */
      void enqueueOrder(EventType eventType,
                        double sentPrice,
                        double sentQuantity,
                        AlgoOrderType orderType,
                        const std::string &clientOrderId,
                        OrderMode side);

      /*!
       * \brief Adds reject event with given parameters to the queue of reject
       *        events to be saved into DB.
       *
       * \param clientOrderId Client order id.
       * \param reason Reason of reject.
       * \param timestamp Time stamp of the reject message from the market side.
       */
      void enqueueRejectEvent(const std::string &clientOrderId,
                              const std::string &reason,
                              boost::posix_time::ptime timestamp);

      /*!
       * \brief Adds incoming message event with given parameters to the queue
       *        of incoming message events to be saved into DB.
       *
       * \param clientOrderId Client order id.
       * \param exchangeOrderId Exchange order id.
       * \param eventType Type of event.
       * \param recvPrice Order filled price.
       * \param recvQuantity Order filled quantity.
       * \param timestamp Time stamp of the reject message from the market side.
       */
      void enqueueIncomingMessageEvent(const std::string &clientOrderId,
                                       const std::string &exchangeOrderId,
                                       EventType eventType,
                                       double recvPrice,
                                       double recvQuantity,
                                       boost::posix_time::ptime timestamp);


      /*
       * \brief Save logic serialization.
       */
      template<class Archive>
      void serialize(Archive &ar, const unsigned int version)
      {
        ar & boost::serialization::base_object<GenericStrategy>(*this);
        ar & _dataSymbol & _mode & _status & _exchangeOrderId;
        ar & _lastBidPrice & _lastAskPrice & _attempts;
        ar & _clOrderIdFirstLeg & _cancelClOrderIdFirstLeg & _clorderIdSecondLegLimit & _clorderIdSecondLegStop & _origClorderIdSecondLeg;
        ar & _clorderIdsSecondLegLimit & _origClorderIdsSecondLegLimit;
        ar & _firstLegFilledQty & _secondLegFilledQty & _firstLegCancelledQty & _secondLegCancelledQty;
        ar & _bisRunning & _tm & _tmStopTime & _algoType;
        ar & _parentOrderPrice & _triggerPrice & _cancelTickCeil & _cancelTickFloor;
        ar & _pointABeforeStart & _lastKnownPrice & _lastState & _userName;
        ar & _priceBid & _priceAsk & _lastSavedTime & _bPointBReachedBeforePointA & _afterRestart;
        char buf[MAX_BUF] = {0};
        size_t size;

        if (Archive::is_loading::value)
        {
          ar & size;
          ar & boost::serialization::make_array<char>(buf, size);
          _algoModel1 = AlgoModel1(buf + sizeof(UNSIGNED_SHORT) + sizeof(UNSIGNED_CHARACTER));
          initStrategyObject();
          // Requesting for subscription of market data. It will subscribed after feed connection
          std::string dataFeedVenueId = Db::Backend::getInstance().dataFeedVenueId();
          IMarketDataHandler *marketDataHandlerPtr = MarketDataHandlerMap::getInstance()
                                                     .get(dataFeedVenueId);
          if (!marketDataHandlerPtr->subscribe(_dataSymbol))
          {
            LOG(ERROR) << "[STRATEGY] Failed to subscribe market data for symbol '" << _dataSymbol << "' " << _algoModel1.GetStrategyId();;
          }
          std::map<std::string, int>::iterator iter = openStrategyAllowanceMap.find(_dataSymbol);
          if (iter == openStrategyAllowanceMap.end())
          {
            openStrategyAllowanceMap[_dataSymbol] = 0;
          }
          if (_algoModel1.GetStatus() == StrategyStatus_OPEN)
          {
            openStrategyAllowanceMap[_dataSymbol]++;
          }
        }
        else
        {
          size = _algoModel1.Serialize(buf);
          ar & size;
          ar & boost::serialization::make_array<char>(buf, size);
        }
      }


      std::shared_ptr<Algo::Db::Strategy> _strategy;

      Db::Instrument _dataSymbol;
      OrderMode      _mode;

      StrategyStatus _status;
      std::string    _exchangeOrderId;

      AlgoModel1     _algoModel1;
      double         _lastBidPrice;
      double         _lastAskPrice;

      int            _attempts;
      std::string    _clOrderIdFirstLeg;
      std::string    _cancelClOrderIdFirstLeg;

      ThreadSafeVector<std::string> _clorderIdsSecondLegLimit;
      ThreadSafeVector<std::string> _origClorderIdsSecondLegLimit;

      std::string    _clorderIdSecondLegLimit;
      std::string    _clorderIdSecondLegStop;
      std::string    _origClorderIdSecondLeg;

      UNSIGNED_LONG  _firstLegFilledQty;
      UNSIGNED_LONG  _secondLegFilledQty;

      UNSIGNED_LONG  _firstLegCancelledQty;
      UNSIGNED_LONG  _secondLegCancelledQty;

      bool           _bisRunning;
      time_t         _tm;
      time_t         _tmStopTime;
      AlgoType       _algoType;

      double         _parentOrderPrice;
      double         _triggerPrice;
      double         _cancelTickCeil;
      double         _cancelTickFloor;

      bool           _pointABeforeStart;

      double         _lastKnownPrice; //Last known price
      AlgoMode1State _lastState;
      std::string    _userName;

      double         _priceBid;
      double         _priceAsk;
      long long      _lastSavedTime;
      bool           _bPointBReachedBeforePointA;  //Only for Algo type 2
  };
}

#endif
