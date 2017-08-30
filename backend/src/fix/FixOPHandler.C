/*
 * =====================================================================================
 *
 *       Filename:  FixOPHandler.C
 *
 *    Description:
 *
 *        Created:  09/23/2016 08:09:19 AM
 *       Compiler:  g++
 *
 *         Author:  Soumya Prasad Ukil
 *        Company:  AlgoEngine
 *
 * =====================================================================================
 */

#include <quickfix/fix44/Logout.h>
#include <quickfix/fix44/Logon.h>
#include <quickfix/fix44/TradingSessionStatus.h>
#include <quickfix/fix44/BusinessMessageReject.h>
#include <quickfix/fix44/NewOrderList.h>
#include <quickfix/fix44/OrderCancelReplaceRequest.h>
#include <shared/order.h>
#include <shared/time_util.h>
#include <shared/easylogging++.h>
#include <fix/FixOPHandler.H>
#include <fix/FixMessageReader.H>
#include <symbol/SymbolDictionary.H>
#include <constant/ServerGlobals.H>
#include <connection/ConnectionStatus.H>
#include <storage/OrderStore.H>
#include <storage/PersistOrder.H>
#include <util/UniqueIdGenerator.H>

namespace Algo
{
  long FixOPHandler::_clOrdId;
  FIX::SessionID FixOPHandler::_sessioId;
  int FixOPHandler::_enableDebugLog;

  void FixOPHandler::start(std::string &fixConfigFile, int enableDebugLog)
  {
    _enableDebugLog = enableDebugLog;
    _settings = new FIX::SessionSettings(fixConfigFile);
    _storeFactory = new FIX::FileStoreFactory(*_settings);
    _logFactory = new FIX::ScreenLogFactory(*_settings);

    _initiator =
      new FIX::ThreadedSocketInitiator(*this,
          *_storeFactory,
          *_settings, *_logFactory);
    _initiator->start();
  }

  void FixOPHandler::stop()
  {
    _initiator->stop();
  }

  bool FixOPHandler::isConnected()
  {
    return _isConnected;
  }

  void FixOPHandler::setStartClOrderId(long id)
  {
    _clOrdId = id;
  }

  std::string FixOPHandler:: getNextId()
  {
    ++_clOrdId;
    std::string str = std::to_string((long long) _clOrdId);
    return str;
  }

  double FixOPHandler::modifiedSecondaryLimitPrice(double marketPrice, double tickSize)
  {
    return marketPrice + tickSize;
  }

  bool FixOPHandler::sendBracketOrder(const std::string & symbol,
                                      double primaryEntryPrice,
                                      double secondaryLimitPrice,
                                      double secondaryStopPrice,
                                      double secondaryStopLimitPrice,
                                      double pointC,
                                      int quantity,
                                      OrderMode side,
                                      double trailing,
                                      UNSIGNED_LONG strategyId,
                                      std::string & firstLegClOrderId,
                                      std::string & secondLegLimitClOrderId,
                                      std::string & secondLegStopClOrderId,
                                      long internalId,
                                      AlgoType algoType,
                                      bool isIAC,
                                      const std::string& upname,
                                      const std::string& unique)
  {
    if (!isIAC) {
      LOG(INFO) << "[FXCM OP] Sending Bracket Order for ["
        << symbol << "] with parent order price(Price tag 44) ["
        << primaryEntryPrice << "] trigger price(StopPx tag 99) ["
        << pointC << "] Stop price secondary order ["
        << secondaryStopPrice << "] Limit Price secondary order ["
        << secondaryLimitPrice << "] for quantity ["
        << quantity << "] strategy id [" << strategyId << "]";
    }
    else {
      LOG(INFO) << "[FXCM OP] Sending Bracket Order IAC for ["
        << symbol << "] with parent order price(Price tag 44) ["
        << primaryEntryPrice << "] Stop price secondary order ["
        << secondaryStopPrice << "] Limit Price secondary order ["
        << secondaryLimitPrice << "] for quantity ["
        << quantity << "] strategy id [" << strategyId << "]";
    }
    Order *firstLeg = new Order();
    Order *secondLegStop = new Order();
    Order *secondLegLimit = new Order();

    firstLeg->setStrategyId(strategyId);
    secondLegLimit->setStrategyId(strategyId);
    secondLegStop->setStrategyId(strategyId);

    firstLeg->setAlgoType(algoType);
    secondLegLimit->setAlgoType(algoType);
    secondLegStop->setAlgoType(algoType);

    std::string postfix("_");
    postfix +=
      (std::to_string(internalId)) + "_" +
      std::to_string(strategyId);
    firstLegClOrderId =
      std::to_string(UniqueIdGenerator::getInstance().
          generateUniqueOrderId()) + postfix;
    secondLegStopClOrderId =
      std::to_string(UniqueIdGenerator::getInstance().
          generateUniqueOrderId()) + postfix;
    secondLegLimitClOrderId =
      std::to_string(UniqueIdGenerator::getInstance().
          generateUniqueOrderId()) + postfix;

    FIX44::NewOrderList olist;

    FIX::Side side1 = FIX::Side_BUY;
    FIX::Side side2 = FIX::Side_SELL;
    firstLeg->setOrderMode(OrderMode_BUY);
    secondLegStop->setOrderMode(OrderMode_SELL);
    secondLegLimit->setOrderMode(OrderMode_SELL);

    firstLeg->setOrderStatus(OrderStatus_PENDING);
    secondLegLimit->setOrderStatus(OrderStatus_PENDING);
    secondLegStop->setOrderStatus(OrderStatus_PENDING);

    firstLeg->setExchangeEntryTime(std::chrono::duration_cast <
        std::chrono::microseconds >
        (std::chrono::system_clock::now().
         time_since_epoch()).count());
    secondLegStop->setExchangeEntryTime(std::chrono::duration_cast <
        std::chrono::microseconds >
        (std::chrono::
         system_clock::now().
         time_since_epoch()).count());
    secondLegLimit->setExchangeEntryTime(std::chrono::duration_cast <
        std::chrono::microseconds >
        (std::chrono::
         system_clock::now().
         time_since_epoch()).count());

    if (side == OrderMode_SELL) {
      side1 = FIX::Side_SELL;
      side2 = FIX::Side_BUY;

      firstLeg->setOrderMode(OrderMode_SELL);
      secondLegStop->setOrderMode(OrderMode_BUY);
      secondLegLimit->setOrderMode(OrderMode_BUY);
    }
    olist.setField(FIX::ListID(getNextId()));
    olist.setField(FIX::TotNoOrders(3));
    olist.setField(FIX::FIELD::ContingencyType, "101");
    olist.setField(FIX::TransactTime(FIX::TransactTime()));

    FIX44::NewOrderList::NoOrders order;

    order.setField(FIX::ClOrdID(firstLegClOrderId));
    firstLeg->setClOrdId(firstLegClOrderId);

    order.setField(FIX::ListSeqNo(1));
    order.setField(FIX::ClOrdLinkID("1"));
    if (side == OrderMode_SELL) {
      order.setField(FIX::Account(_accountSell));
      firstLeg->setAccount(_accountSell);
    }
    else {
      order.setField(FIX::Account(_accountBuy));
      firstLeg->setAccount(_accountBuy);
    }
    order.setField(FIX::Symbol(symbol));
    firstLeg->setSymbol(symbol);

    order.setField(FIX::Side(side1));

    order.setField(FIX::OrderQty(quantity));
    firstLeg->setQuantity(quantity);
    if (isIAC) {
      order.setField(FIX::OrdType(FIX::OrdType_LIMIT));
      firstLeg->setOrderType(OrderType_LIMIT);
    }
    else {
      order.setField(FIX::OrdType(FIX::OrdType_STOP_LIMIT));
      firstLeg->setOrderType(OrderType_STOP_LIMIT);
    }
    if (isIAC) {
      order.setField(FIX::TimeInForce
          (FIX::TimeInForce_IMMEDIATE_OR_CANCEL));
      firstLeg->setOrderValidity(TimeInForce_IOC);
    }
    else {
      order.setField(FIX::TimeInForce
          (FIX::TimeInForce_GOOD_TILL_CANCEL));
      firstLeg->setOrderValidity(TimeInForce_GTC);
    }

    int precision = SymbolDictionary::getInstance().getPrecision(symbol);

    //99 -> StopPx
    //44 -> Price
    order.setField(44, std::to_string(primaryEntryPrice).substr(0, precision));
    firstLeg->setPrice(primaryEntryPrice);

    olist.addGroup(order);

    FIX44::NewOrderList::NoOrders stop;
    stop.setField(FIX::ClOrdID(secondLegStopClOrderId));
    secondLegStop->setClOrdId(secondLegStopClOrderId);

    stop.setField(FIX::ListSeqNo(2));
    stop.setField(FIX::ClOrdLinkID("2"));

    int buy_or_sell = -1;
    if (side == OrderMode_SELL) {
      stop.setField(FIX::Account(_accountSell));
      secondLegStop->setAccount(_accountSell);
      buy_or_sell = 1;
    }
    else {
      stop.setField(FIX::Account(_accountBuy));
      secondLegStop->setAccount(_accountBuy);
    }

    stop.setField(FIX::Side(side2));
    stop.setField(FIX::Symbol(symbol));
    secondLegStop->setSymbol(symbol);

    stop.setField(FIX::OrderQty(quantity));
    secondLegStop->setQuantity(quantity);

    stop.setField(FIX::OrdType(FIX::OrdType_STOP));
    secondLegStop->setOrderType(OrderType_STOP);

    stop.setField(99,
        std::to_string(secondaryStopPrice).substr(0, precision));
    secondLegStop->setPrice(secondaryStopPrice);

    stop.setField(FIX::PegOffsetValue(buy_or_sell * (float) trailing));

    stop.setField(1094, "4");
    stop.setField(9061, std::to_string(trailing));

    olist.addGroup(stop);

    FIX44::NewOrderList::NoOrders limit;
    limit.setField(FIX::ClOrdID(secondLegLimitClOrderId));
    secondLegLimit->setClOrdId(secondLegLimitClOrderId);

    limit.setField(FIX::ListSeqNo(3));
    limit.setField(FIX::ClOrdLinkID("2"));

    if (side == OrderMode_SELL) {
      limit.setField(FIX::Account(_accountSell));
      secondLegLimit->setAccount(_accountSell);
    }
    else {
      limit.setField(FIX::Account(_accountBuy));
      secondLegLimit->setAccount(_accountBuy);
    }

    limit.setField(FIX::Side(side2));
    limit.setField(FIX::Symbol(symbol));
    secondLegLimit->setSymbol(symbol);

    limit.setField(FIX::OrderQty(quantity));
    secondLegLimit->setQuantity(quantity);

    limit.setField(FIX::OrdType(FIX::OrdType_LIMIT));
    secondLegLimit->setOrderType(OrderType_LIMIT);

    limit.setField(44,
        std::to_string(secondaryLimitPrice).substr(0, precision));
    secondLegLimit->setPrice(secondaryLimitPrice);

    limit.setField(FIX::
        TimeInForce(FIX::TimeInForce_GOOD_TILL_CANCEL));
    secondLegLimit->setOrderValidity(TimeInForce_GTC);

    olist.addGroup(limit);

    firstLeg->setUniqueStrategyIdentifier(unique);
    firstLeg->setUploadFileName(upname);
    secondLegLimit->setUniqueStrategyIdentifier(unique);
    secondLegLimit->setUploadFileName(upname);
    secondLegStop->setUniqueStrategyIdentifier(unique);
    secondLegStop->setUploadFileName(upname);

    OrderStore::getInstance().
      addOrderIntoClOrderIdToOrder(firstLegClOrderId, firstLeg);
    PersistOrder::getInstance().storeOrders(firstLegClOrderId,
        firstLeg->toString());
    OrderStore::getInstance().
      addOrderIntoClOrderIdToOrder(secondLegLimitClOrderId,
          secondLegLimit);
    PersistOrder::getInstance().storeOrders(secondLegLimitClOrderId,
        secondLegLimit->toString
        ());
    OrderStore::getInstance().
      addOrderIntoClOrderIdToOrder(secondLegStopClOrderId,
          secondLegStop);
    PersistOrder::getInstance().storeOrders(secondLegStopClOrderId,
        secondLegStop->toString());

    OrderStore::getInstance().
      addStrategyIdIntoClOrderIdToStretgyId(firstLegClOrderId,
          strategyId);
    OrderStore::getInstance().
      addStrategyIdIntoClOrderIdToStretgyId(secondLegLimitClOrderId,
          strategyId);
    OrderStore::getInstance().
      addStrategyIdIntoClOrderIdToStretgyId(secondLegStopClOrderId,
          strategyId);

    if (_enableDebugLog) {
      FixMessageReader::deserialize("OUTBOUND [FXCM OP]",
          olist.toString());
    }
    try {                   // Check for valid session id...
      LOG(INFO) << "[FXCM OP] Sending Message " << olist.toString();
      return FIX::Session::sendToTarget(olist, _sessioId);
    }
    catch(FIX::SessionNotFound & e) {
      LOG(INFO) << "[FXCM OP] Cannot send order " << e.what();
    }
    catch( ...) {
      LOG(ERROR) <<
        "[FXCM OP] Unknown exception in send order. Cannot send order ";
    }
    return false;
  }
  bool FixOPHandler::modifyOrder(
      const std::string& originalOrderId,
      std::string& newOrderId,
      double price,
      double qty,
      long internalId){
    Order *order = OrderStore::getInstance().getOrderFromClOrderIdToOrder(originalOrderId);
    if(!order){
      LOG(ERROR) << "Modify Order: Could not find order with client order id [" << originalOrderId <<"] in order book.";
      return false;
    }
    std::string postfix("_");
    postfix += (std::to_string(internalId)) + "_" + std::to_string(order->getStrategyId());
    newOrderId =
      std::to_string(
          UniqueIdGenerator::getInstance().generateUniqueOrderId()) +
      postfix;
    order->setSecondaryOrderId(newOrderId);

    FIX::Side side = FIX::Side_BUY;
    if (order->getOrderMode() == OrderMode_SELL) {
      side = FIX::Side_SELL;
    }
    FIX44::OrderCancelReplaceRequest replaceRequest(
        FIX::OrigClOrdID(originalOrderId),FIX::ClOrdID(newOrderId),
        side,FIX::TransactTime(), FIX::OrdType(FIX::OrdType_LIMIT));

    replaceRequest.set(FIX::HandlInst('1'));
    order->setOrigClOrdId(originalOrderId);
    replaceRequest.set(FIX::Account(order->getAccount()));


    order->setExchangeEntryTime(
        std::chrono::duration_cast<std::chrono::microseconds>(
          std::chrono::system_clock::now().time_since_epoch())
        .count());
    replaceRequest.setField(FIX::ListID(getNextId()));
    replaceRequest.setField(FIX::ClOrdLinkID("1"));
    replaceRequest.setField(FIX::TimeInForce(FIX::TimeInForce_GOOD_TILL_CANCEL));
    replaceRequest.set(FIX::Symbol(order->getSymbol()));
    replaceRequest.set(FIX::Price(price));
    order->setPrice(price);
    replaceRequest.set(FIX::OrderID(order->getExchangeOrderId()));
    replaceRequest.set(FIX::OrderQty(qty));
    order->setOldQuantity(order->getQuantity());
    order->setQuantity(qty);

    OrderStore::getInstance().addOrderIntoClOrderIdToOrder(
        newOrderId, order);
    PersistOrder::getInstance().storeOrders(newOrderId,
        order->toString());

    OrderStore::getInstance().addStrategyIdIntoClOrderIdToStretgyId(
        newOrderId, order->getStrategyId());

    if (_enableDebugLog) {
      FixMessageReader::deserialize("OUTBOUND [FXCM OP]", replaceRequest.toString());
    }
    try { // Check for valid session id...
      LOG(INFO) << "[FXCM OP] Sending Message " << replaceRequest.toString();
      return FIX::Session::sendToTarget(replaceRequest, _sessioId);
    } catch (FIX::SessionNotFound &e) {
      LOG(INFO) << "[FXCM OP] Cannot send order " << e.what();
    } catch (...) {
      LOG(ERROR)
        << "[FXCM OP] Unknown exception in send order. Cannot send order ";
    }
    return false;
  }

  FixOPHandler::FixOPHandler()
  {
    _clOrdId = 1000;
  }

  void FixOPHandler::onCreate(const FIX::SessionID &sessionID)
  {
  }

  void FixOPHandler::onLogon(const FIX::SessionID &sessionID)
  {
    LOG(INFO) << "[FXCM OP] Logon received";
    setSessionInf(sessionID);
    ConnectionStatus::getInstance().setOPConnected(true);
  }

  void FixOPHandler::onLogout(const FIX::SessionID &sessionID)
  {
  }

  void FixOPHandler::toAdmin(FIX::Message & message, const FIX::SessionID & sessionId)
  {
    FIX::MsgType msgType;
    message.getHeader().getField(msgType);

    setHeader(message, sessionId);
    if (((msgType.getValue())) == (FIX::MsgType_Logon)) {
      const FIX::Dictionary & sessionSettings =
        _settings->get(sessionId);
      if (sessionSettings.has("Username")) {
        message.setField(FIX::Username
            (sessionSettings.getString("Username")));
      }
      if (sessionSettings.has("Password")) {
        message.setField(FIX::Password
            (sessionSettings.getString("Password")));
      }
      //message.setField(FIX::Account(_account));

      message.setField(FIX::EncryptMethod(0));
      message.getHeader().setField(FIX::MsgSeqNum(1));
      message.setField(FIX::
          ResetSeqNumFlag(FIX::ResetSeqNumFlag_YES));
      LOG(INFO) << "[FXCM OP] Sending logon message s" <<
        message.toString();
    }
  }

  void FixOPHandler::setHeader(FIX::Message & message, const FIX::SessionID & sessionId)
  {
    message.getHeader().setField(FIX::SenderCompID
        (_settings->get(sessionId).getString
         ("SenderCompID")));
    message.getHeader().setField(FIX::TargetCompID
        (_settings->get(sessionId).getString
         ("TargetCompID")));
    message.getHeader().setField(FIX::TargetSubID
        (_settings->
         get(sessionId).getString
         ("TargetSubID")));
  }

  void FixOPHandler::fromAdmin (const FIX::Message &message, const FIX::SessionID&) throw(
      FIX::FieldNotFound,
      FIX::IncorrectDataFormat,
      FIX::IncorrectTagValue,
      FIX::RejectLogon)
  {
    if (_enableDebugLog)
    {
      FixMessageReader::deserialize("INBOUND [FXCM OP]", message.toString ());
    }
  }

  void FixOPHandler::toApp (FIX::Message & message, const FIX::SessionID &) throw (FIX::DoNotSend)
  {
  }

  void FixOPHandler::fromApp(const FIX::Message & message, const FIX::SessionID &sessionID) throw (
      FIX::FieldNotFound,
      FIX::IncorrectDataFormat,
      FIX::IncorrectTagValue,
      FIX::UnsupportedMessageType)
  {
    LOG(INFO) << "IN : " << message.toString();
    crack(message, sessionID);
  }

  void FixOPHandler::onMessage(const FIX44::Logout &message, const FIX::SessionID &sessionID)
  {
    LOG(INFO) << "[FXCM OP] Logout received";
    _isConnected = false;
  }

  void FixOPHandler::onMessage(const FIX44::Logon &message,const FIX::SessionID &sessionID)
  {
    if (_enableDebugLog) {
      FixMessageReader::deserialize("INBOUND [FXCM OP]", message.toString());
    }
    LOG(INFO) << "[FXCM OP] Logon received";
    LOG(INFO) << "[FXCM OP] Message received " << message.toString();
    _sessioId = sessionID;
    _isConnected = true;
  }

  void FixOPHandler::onMessage(const FIX44::TradingSessionStatus &message, const FIX::SessionID &sessionID)
  {
    if (_enableDebugLog) {
      FixMessageReader::deserialize("INBOUND [FXCM OP]", message.toString());
    }
    LOG(INFO) << "[FXCM OP] Message received " << message.toString();
    LOG(INFO) << "[FXCM OP] Received Trading session status";
  }

  void FixOPHandler::onMessage(const FIX44::BusinessMessageReject &message, const FIX::SessionID &sessionID)
  {
    if (_enableDebugLog) {
      FixMessageReader::deserialize("INBOUND [FXCM OP]", message.toString());
    }
    LOG(INFO) << "[FXCM OP] Message received " << message.toString();
    LOG(WARNING) << "[FXCM OP] BusinessMessageReject received";
  }

  void FixOPHandler::onMessage(const FIX44::OrderCancelReject &message, const FIX::SessionID&)
  {
    if (_enableDebugLog) {
      FixMessageReader::deserialize("INBOUND [FXCM OP]",  message.toString());
    }
    LOG(INFO) << "[FXCM OP] Replace Reject Received Message " <<
      message.toString();
    FIX::OrderID oid;
    message.getField(oid);
    LOG(INFO) << "[Tag 37] Order Id = " << oid;

    std::string clOrderId = message.getField(11);   //clOrdId
    Order *order =
      OrderStore::getInstance().
      getOrderFromClOrderIdToOrder(clOrderId);
    if (order) {
      FIX::OrdStatus s;
      message.get(s);
      order->setOrderStatus((FIX::OrdStatus) message.getField(39)[0]);    //OrderStatus
      long strategyId = 0;

      if (OrderStore::getInstance().
          getStrategyIdFromClOrderIdToStretgyId(clOrderId,
            strategyId)) {
        order->setExchangeOrderId(oid); //OrderId
        order->setExecType('Z'); //Need to find a better way. This is replace reject
        std::string reason = message.getField(9029);
        order->setRejectReason(reason);
        FIX::SendingTime sendingTime;
        message.getHeader().getField(sendingTime);
        order->setExchangeExitTime(sendingTime.getString());
        OrderStore::getInstance().insertOrder(*order);

        PersistOrder::getInstance().storeOrders(clOrderId, order->toString());
        while (!gSpscOrderMsgQueue.push(*order));
        order->dumpOrder();
      }
      else {
        LOG(WARNING) << "[FXCM OP] Strategy terminated Client Order Id [" << clOrderId << "] does not exist in the ClOrderIdToStretgyId map";
      }
    }
    else {
      LOG(WARNING) << "[FXCM OP] Client Order Id [" << clOrderId << "] does not exist in the ClOrderIdToOrder map";
    }
  }

  void FixOPHandler::onMessage(const FIX44::ExecutionReport &message, const FIX::SessionID&)
  {
    if (_enableDebugLog) {
      FixMessageReader::deserialize("INBOUND [FXCM OP]",  message.toString());
    }
    LOG(INFO) << "[FXCM OP] ExecutionReport Received Message " <<
      message.toString();
    FIX::OrderID oid;
    message.getField(oid);
    LOG(INFO) << "[Tag 37] Order Id = " << oid;

    std::string clOrderId = message.getField(11);   //clOrdId

    Order *order =
      OrderStore::getInstance().
      getOrderFromClOrderIdToOrder(clOrderId);
    if (order) {
      FIX::OrdStatus s;
      message.get(s);
      order->setOrderStatus((FIX::OrdStatus) message.getField(39)[0]);    //OrderStatus
      long strategyId = 0;

      if (OrderStore::getInstance().
          getStrategyIdFromClOrderIdToStretgyId(clOrderId,
            strategyId)) {
        order->setExchangeOrderId(oid); //OrderId

        FIX::ExecType execType;
        // See what kind of execution report this is:
        message.get(execType);
        if ((FIX::ExecType_FILL == execType.getValue()
              || FIX::ExecType_PARTIAL_FILL == execType.getValue()
              || FIX::ExecType_TRADE == execType.getValue()
            ) && FIX::OrdStatus_FILLED == s) {
          FIX::LastQty lastQty;
          FIX::LastPx lastPx;
          FIX::SendingTime sendingTime;
          message.get(lastQty);
          message.get(lastPx);
          message.getHeader().getField(sendingTime);
          LOG(INFO) << "[FXCM OP] Order Filled for [" <<
            clOrderId << "] for Qty [" << lastQty << "] @ [" <<
            lastPx << "]";
          order->setFilledPrice(lastPx);      //Last Fill Price
          order->setFilledQuantity(std::stol(message.getField(14)));  //CumQty
          order->setRemaining(std::stol(message.getField(151)));      //LeavesQty
          order->setExchangeExitTime(sendingTime.getString());

          order->setExecType('F');
        }
        else if(FIX::ExecType_REPLACE == execType.getValue() && FIX::OrdStatus_REPLACED == s)
        {
          FIX::LastQty lastQty;
          FIX::LastPx lastPx;
          FIX::SendingTime sendingTime;
          message.get(lastQty);
          message.get(lastPx);
          message.getHeader().getField(sendingTime);
          LOG(INFO) << "[FXCM OP] Replaced Order Filled for [" << clOrderId <<"] for Qty ["
            <<lastQty<<"] @ ["<<lastPx<<"]";
          order->setFilledPrice(lastPx); //Last Fill Price
          order->setFilledQuantity(std::stol(message.getField(14))); //CumQty
          order->setRemaining(std::stol(message.getField(151))); //LeavesQty
          order->setExchangeExitTime(sendingTime.getString());

          order->setExecType('5');
        }
        else if (FIX::ExecType_NEW == execType.getValue()) {
          LOG(INFO) << "[FXCM OP] New accepted for Order Id [" <<
            clOrderId << "]";
          FIX::SendingTime sendingTime;
          FIX::OrderQty orderQty;
          FIX::Price price;
          message.getHeader().get(sendingTime);
          message.get(orderQty);
          message.get(price);
          order->setExchangeExitTime(sendingTime.getString());
          order->setAckedPrice(price);
          order->setAckedQuantity(orderQty);
          order->setExecType('A');
        }
        else if (FIX::OrdStatus_STOPPED == s) {
          LOG(INFO) << "[FXCM OP] Order [" << clOrderId <<
            "] Stopped";
          order->setExecType('S');
        }
        else if (FIX::ExecType_REJECTED == execType.getValue()) {
          LOG(INFO) << "[FXCM OP] New Reject for order id [" <<
            clOrderId << "]";
          FIX::OrderQty orderQty;
          message.get(orderQty);
          order->setCancelledQty(orderQty);
          order->setExecType('R');
          std::string reason = message.getField(9029);
          order->setRejectReason(reason);
          FIX::SendingTime sendingTime;
          message.getHeader().getField(sendingTime);
          order->setExchangeExitTime(sendingTime.getString());
        }
        else if (FIX::ExecType_CANCELLED == execType.getValue()) {
          LOG(INFO) << "[FXCM OP] Order ID [" << clOrderId <<
            "] successfully canceled.";
          FIX::OrderQty orderQty;
          message.get(orderQty);
          order->setCancelledQty(orderQty);
          order->setExecType('C');
          FIX::SendingTime sendingTime;
          message.getHeader().getField(sendingTime);
          order->setExchangeExitTime(sendingTime.getString());
        }
        else if (FIX::ExecType_PENDING_CANCEL ==
            execType.getValue()) {
          LOG(INFO) << "[FXCM OP] Pending Cancel for order id ["
            << clOrderId << "]";
          return;
        }
        else {
          LOG(WARNING) <<
            "[FXCM OP] Unhandled Exec type FIX Message [" <<
            message.toString() << "]";
          return;
        }
        OrderStore::getInstance().
          addOrderIntoClOrderIdToOrder(clOrderId, order);

        //Keeping track of only Filled,Cancelled or Rejected orders.
        if (FIX::OrdStatus_STOPPED != s
            || FIX::ExecType_NEW != execType.getValue())
          OrderStore::
            getInstance().insertOrder(*order);

        PersistOrder::getInstance().storeOrders(clOrderId,
            order->toString());

        while (!gSpscOrderMsgQueue.push(*order));
        order->dumpOrder();
      }
      else {
        LOG(WARNING) <<
          "[FXCM OP] Strategy terminated Client Order Id [" <<
          clOrderId <<
          "] does not exist in the ClOrderIdToStretgyId map";
      }
    }
    else {
      LOG(WARNING) << "[FXCM OP] Client Order Id [" << clOrderId
        << "] does not exist in the ClOrderIdToOrder map";
    }
  }

  void FixOPHandler::setSessionInf(const FIX::SessionID &sessionId)
  {
    _sessioId = sessionId;
  }
}
