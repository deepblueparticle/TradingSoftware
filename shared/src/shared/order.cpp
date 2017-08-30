#include <shared/order.h>
#include <iostream>
#include <sstream>
#include <algorithm>
#ifndef FRONTEND
#include <shared/easylogging++.h>
#else
#include <QDebug>
#include <QString>
#endif
namespace Algo {
    Order::Order() :
            _transactionType(TransactionType_MAX),
            _clOrdId(""),
            _origClOrdId(""),
            _exchangeOrderId(""),
            _parentOrderId(""),
            _secondaryOrderId(""),
            _symbol(""),
            _orderMode(OrderMode_MAX),
            _quantity(0),
            _disclosedQuantity(0),
            _filledPrice(0),
            _filledQuantity(0),
            _ackedPrice(0),
            _ackedQuantity(0),
            _oldQuantity(0),
            _price(0),
            _stopPrice(0),
            _orderValidity(TimeInForce_MAX),
            _orderType(OrderType_MAX),
            _orderStatus(OrderStatus_MAX),
            _exchangeEntryTime(0),
            _account(""),
            _strategyId(0),
            _algoType(Algo_MAX),
            _cancelledQty(0),
            _remaining(0),
            _execType('\0'),
            _rejectReason(""),
            _uploadFileName(""),
            _exchangeExitTime(""),
            _uniqueStrategyIdentifier(""),
            _exchangeStatus("") {
    }

    // copy constructor
    Order::Order(const Order &other) {
        _transactionType = other._transactionType;
        _clOrdId = other._clOrdId;
        _origClOrdId = other._origClOrdId;
        _exchangeOrderId = other._exchangeOrderId;
        _symbol = other._symbol;
        _orderMode = other._orderMode;
        _quantity = other._quantity;
        _disclosedQuantity = other._disclosedQuantity;
        _filledPrice = other._filledPrice;
        _filledQuantity = other._filledQuantity;
        _ackedPrice = other._ackedPrice;
        _ackedQuantity = other._ackedQuantity;
        _oldQuantity = other._oldQuantity;
        _price = other._price;
        _stopPrice = other._stopPrice;
        _orderValidity = other._orderValidity;
        _orderType = other._orderType;
        _orderStatus = other._orderStatus;
        _exchangeEntryTime = other._exchangeEntryTime;
        _account = other._account;
        _strategyId = other._strategyId;
        _algoType = other._algoType;
        _cancelledQty = other._cancelledQty;
        _remaining = other._remaining;
        _execType = other._execType;
        _rejectReason = other._rejectReason;
        _exchangeExitTime = other._exchangeExitTime;
        _uniqueStrategyIdentifier = other._uniqueStrategyIdentifier;
        _uploadFileName = other._uploadFileName;
        _exchangeStatus = other._exchangeStatus;
        _parentOrderId = other._parentOrderId;
        _secondaryOrderId = other._secondaryOrderId;
    }

    // copy assignment operator
    Order &Order::operator=(Order &other) {

        // check for self-assignment by comparing the address of the
        // implicit object and the parameter
        if (this == &other)
            return *this;

        // do the copy
        _transactionType = other._transactionType;
        _clOrdId = other._clOrdId;
        _origClOrdId = other._origClOrdId;
        _exchangeOrderId = other._exchangeOrderId;
        _symbol = other._symbol;
        _orderMode = other._orderMode;
        _quantity = other._quantity;
        _disclosedQuantity = other._disclosedQuantity;
        _filledPrice = other._filledPrice;
        _filledQuantity = other._filledQuantity;
        _ackedPrice = other._ackedPrice;
        _ackedQuantity = other._ackedQuantity;
        _oldQuantity = other._oldQuantity;
        _price = other._price;
        _stopPrice = other._stopPrice;
        _orderValidity = other._orderValidity;
        _orderType = other._orderType;
        _orderStatus = other._orderStatus;
        _exchangeEntryTime = other._exchangeEntryTime;
        _account = other._account;
        _strategyId = other._strategyId;
        _algoType = other._algoType;
        _cancelledQty = other._cancelledQty;
        _remaining = other._remaining;
        _execType = other._execType;
        _rejectReason = other._rejectReason;
        _exchangeExitTime = other._exchangeExitTime;
        _uniqueStrategyIdentifier = other._uniqueStrategyIdentifier;
        _uploadFileName = other._uploadFileName;
        _exchangeStatus = other._exchangeStatus;
        _parentOrderId = other._parentOrderId;
        _secondaryOrderId = other._secondaryOrderId;

        // return the existing object
        return *this;
    }

    Order::Order(Order &&other) //the canonical signature of a move constructor
    //first, set the target members to their default values
            : _transactionType(TransactionType_MAX),
              _clOrdId(""),
              _origClOrdId(""),
              _exchangeOrderId(""),
              _symbol(""),
              _orderMode(OrderMode_MAX),
              _quantity(0),
              _disclosedQuantity(0),
              _filledPrice(0),
              _filledQuantity(0),
              _ackedPrice(0),
              _ackedQuantity(0),
              _oldQuantity(0),
              _price(0),
              _stopPrice(0),
              _orderValidity(TimeInForce_MAX),
              _orderType(OrderType_MAX),
              _orderStatus(OrderStatus_MAX),
              _exchangeEntryTime(0),
              _account(""),
              _secondaryOrderId(""),
              _strategyId(0),
              _algoType(Algo_MAX),
              _cancelledQty(0),
              _remaining(0),
              _execType('\0'),
              _rejectReason(""),
              _uploadFileName(""),
              _exchangeExitTime(""),
              _uniqueStrategyIdentifier(""),
              _exchangeStatus("")
    {
        //next, pilfer the source's resources
        _clOrdId = other._clOrdId;
        _origClOrdId = other._origClOrdId;
        _exchangeOrderId = other._exchangeOrderId;
        _quantity = other._quantity;
        _disclosedQuantity = other._disclosedQuantity;
        _oldQuantity = other._oldQuantity;
        _price = other._price;
        _stopPrice = other._stopPrice;
        _exchangeEntryTime = other._exchangeEntryTime;
        _orderMode = other._orderMode;
        _transactionType = other._transactionType;
        _orderValidity = other._orderValidity;
        _orderType = other._orderType;
        _orderStatus = other._orderStatus;
        _symbol = other._symbol;
        _filledPrice = other._filledPrice;
        _filledQuantity = other._filledQuantity;
        _ackedPrice = other._ackedPrice;
        _ackedQuantity = other._ackedQuantity;
        _account = other._account;
        _strategyId = other._strategyId;
        _algoType = other._algoType;
        _cancelledQty = other._cancelledQty;
        _remaining = other._remaining;
        _execType = other._execType;
        _rejectReason = other._rejectReason;
        _exchangeExitTime = other._exchangeExitTime;
        _uniqueStrategyIdentifier = other._uniqueStrategyIdentifier;
        _uploadFileName = other._uploadFileName;
        _exchangeStatus = other._exchangeStatus;
        _parentOrderId = other._parentOrderId;
        _secondaryOrderId = other._secondaryOrderId;


        //finally, set the source's data members to default values to prevent aliasing
        other._clOrdId = "";
        other._origClOrdId = "";
        other._exchangeOrderId = "";
        other._quantity = 0;
        other._disclosedQuantity = 0;
        other._oldQuantity = 0;
        other._price = 0;
        other._stopPrice = 0;
        other._exchangeEntryTime = 0;
        other._orderMode = OrderMode_MAX;
        other._transactionType = TransactionType_MAX;
        other._orderValidity = TimeInForce_MAX;
        other._orderType = OrderType_MAX;
        other._orderStatus = OrderStatus_MAX;
        other._symbol = "";
        other._filledPrice = 0;
        other._filledQuantity = 0;
        other._ackedPrice = 0;
        other._ackedQuantity = 0;
        other._account = "";
        other._strategyId = 0;
        other._algoType = Algo_MAX;
        other._cancelledQty = 0;
        other._remaining = 0;
        other._execType = '\0';
        other._exchangeExitTime = "";
        other._rejectReason = "";
        other._uniqueStrategyIdentifier = "";
        other._uploadFileName = "";
        other._exchangeStatus = "";
        other._parentOrderId = "";
        other._secondaryOrderId = "";

    }

    Order &Order::operator=(Order &&other) //the canonical signature of a move assignment operator
    {
        if (this != &other) {
            //next, pilfer the source's resources

            //next, pilfer the source's resources
            _clOrdId = other._clOrdId;
            _origClOrdId = other._origClOrdId;
            _exchangeOrderId = other._exchangeOrderId;
            _quantity = other._quantity;
            _disclosedQuantity = other._disclosedQuantity;
            _oldQuantity = other._oldQuantity;
            _price = other._price;
            _stopPrice = other._stopPrice;
            _exchangeEntryTime = other._exchangeEntryTime;
            _orderMode = other._orderMode;
            _transactionType = other._transactionType;
            _orderValidity = other._orderValidity;
            _orderType = other._orderType;
            _orderStatus = other._orderStatus;
            _symbol = other._symbol;
            _filledPrice = other._filledPrice;
            _filledQuantity = other._filledQuantity;
            _ackedPrice = other._ackedPrice;
            _ackedQuantity = other._ackedQuantity;
            _account = other._account;
            _strategyId = other._strategyId;
            _algoType = other._algoType;

            _cancelledQty = other._cancelledQty;
            _remaining = other._remaining;
            _execType = other._execType;
            _rejectReason = other._rejectReason;
            _exchangeExitTime = other._exchangeExitTime;
            _uniqueStrategyIdentifier = other._uniqueStrategyIdentifier;
            _uploadFileName = other._uploadFileName;
            _exchangeStatus = other._exchangeStatus;
            _parentOrderId = other._parentOrderId;
            _secondaryOrderId = other._secondaryOrderId;

            //finally, set the source's data members to default values to prevent aliasing
            other._clOrdId = "";
            other._origClOrdId = "";
            other._exchangeOrderId = "";
            other._quantity = 0;
            other._disclosedQuantity = 0;
            other._oldQuantity = 0;
            other._price = 0;
            other._stopPrice = 0;
            other._exchangeEntryTime = 0;
            other._orderMode = OrderMode_MAX;
            other._transactionType = TransactionType_MAX;
            other._orderValidity = TimeInForce_MAX;
            other._orderType = OrderType_MAX;
            other._orderStatus = OrderStatus_MAX;
            other._symbol = "";
            other._filledPrice = 0;
            other._filledQuantity = 0;
            other._ackedPrice = 0;
            other._ackedQuantity = 0;
            other._account = "";
            other._strategyId = 0;
            other._algoType = Algo_MAX;
            other._cancelledQty = 0;
            other._remaining = 0;
            other._execType = 0;
            other._rejectReason = "";
            other._exchangeExitTime = "";
            other._uniqueStrategyIdentifier = "";
            other._uploadFileName = "";
            other._exchangeStatus = "";
            other._parentOrderId = "";
            other._secondaryOrderId = "";

        }
        return *this;
    }


#ifndef FRONTEND
    void Order::dumpOrder() {
        LOG(INFO) << "Order Details...... ";
        LOG(INFO) << "ClOrdID............ " << this->getClOrdId();
        LOG(INFO) << "OrigClOrdId........ " << this->getOrigClOrdId();
        LOG(INFO) << "exchangeOrderId.... " << this->getExchangeOrderId();
        LOG(INFO) << "Symbol............. " << this->getSymbol();
        LOG(INFO) << "OrderMode.......... " << this->getOrderMode();
        LOG(INFO) << "Transaction Type... " << this->getTransactionType();
        LOG(INFO) << "Quantity........... " << this->getQuantity();
        LOG(INFO) << "FilledPrice........ " << this->getFilledPrice();
        LOG(INFO) << "FilledQuantity..... " << this->getFilledQuantity();
        LOG(INFO) << "OldQuantity........ " << this->getOldQuantity();
        LOG(INFO) << "Price.............. " << this->getPrice();
        LOG(INFO) << "OrderValidity...... " << this->getOrderValidity();
        LOG(INFO) << "OrderStatus........ " << this->getOrderStatus();
        LOG(INFO) << "OrderType.......... " << this->getOrderType();
        LOG(INFO) << "Exch EntryTime..... " << this->getExchangeEntryTime();
        LOG(INFO) << "Exch ExitTime..... " << this->getExchangeExitTime();
        LOG(INFO) << "Account............ " << this->getAccount();
        LOG(INFO) << "Strategy Id........ " << this->getStrategyId();
        LOG(INFO) << "Algo Type.......... " << this->getAlgoType();
        LOG(INFO) << "Remaining Qty...... " << this->getRemainingQty();
        LOG(INFO) << "Cancelled Qty...... " << this->getCancelledQty();
        LOG(INFO) << "Exchange Status.... " << this->getExchangeStatus();
        LOG(INFO) << "Parent OrderId..... " << this->getParentOrderId();
        LOG(INFO) << "Secondary OrderId..... " << this->getSecondaryOrderId();

    }
#endif
    bool Order::fromString(const std::string& str, Order& order) {

        std::istringstream ss(str);
        std::string token;
        //44492,,44492,44491,6EM7,2,3,1,0,0,0,1.085,0,confirmed,0,1492703799143327,,10,2,0,0,Final_JP INPUT FILE_ USE THIS,1,PreSubmitted,44491,,
        // 2109832_14_9,,63413993,USD/CAD,0,3,1000,0,0,0,1.31385,0,confirmed,
        // 0,1483709832846808,6444012827,9,1,0,0,DAILY 20170106_A1_R1,14,PreSubmitted
        if(std::count(str.begin(), str.end(), ',') != 26) // 24 means there are 24 fields below, 
                                                          //do change it if you change number of fields!
            return false;
        std::getline(ss, token, ',');
        order.setClOrdId(token);

        std::getline(ss, token, ',');
        order.setOrigClOrdId(token);

        std::getline(ss, token, ',');
        order.setExchangeOrderId(token);

        std::getline(ss, token, ',');
        order.setParentOrderId(token);

        std::getline(ss, token, ',');
        order.setSymbol(token);

        std::getline(ss, token, ',');
        order.setOrderMode((OrderMode)(token[0]-'0'));
        //3,6000,0,0,0,120.585,0,1,0,0,1000662639,1324206,0,0,

        std::getline(ss, token, ',');
        order.setTransactionType(TransactionType(std::stoi(token)));

        std::getline(ss, token, ',');
        order.setQuantity(std::stoll(token));

        std::getline(ss, token, ',');
        order.setFilledPrice(std::stold(token));

        std::getline(ss, token, ',');
        order.setFilledQuantity(std::stoll(token));

        std::getline(ss, token, ',');
        order.setOldQuantity(std::stoll(token));
        std::getline(ss, token, ',');
        order.setPrice(std::stold(token));

        std::getline(ss, token, ',');
        order.setOrderValidity(TimeInForce(stoi(token)));

        std::getline(ss, token, ',');
        order.setOrderStatus(OrderStatus(std::find(OrderStatusStr, OrderStatusStr+15, token)-OrderStatusStr));

        std::getline(ss, token, ',');
        order.setOrderType(OrderType(stoi(token)));

        std::getline(ss, token, ',');
        order.setExchangeEntryTime(std::stoll(token));

        std::getline(ss, token, ',');
        order.setAccount(token);

        std::getline(ss, token, ',');
        order.setStrategyId(std::stol(token));

        std::getline(ss, token, ',');
        order.setAlgoType(AlgoType(stoi(token)));

        std::getline(ss, token, ',');
        order.setRemaining(std::stol(token));

        std::getline(ss, token, ',');
        order.setCancelledQty(std::stol(token));

        std::getline(ss, token, ',');
        order.setUploadFileName(token);

        std::getline(ss, token, ',');
        order.setUniqueStrategyIdentifier(token);

        std::getline(ss, token, ',');
        order.setExchangeStatus(token);

        std::getline(ss, token, ',');
        order.setParentOrderId(token);

        std::getline(ss, token, ',');
        order.setSecondaryOrderId(token);

        return true;
    }

    std::string Order::toString() const {
        std::ostringstream oss; //Do remeber to change the fromstring if you change this
        oss << this->getClOrdId() << ",";
        oss << this->getOrigClOrdId() << ",";
        oss << this->getExchangeOrderId() << ",";
        oss << this->getParentOrderId() << ",";
        oss << this->getSymbol() << ",";
        oss << this->getOrderMode() << ",";
        oss << this->getTransactionType() << ",";
        oss << this->getQuantity() << ",";
        oss << this->getFilledPrice() << ",";
        oss << this->getFilledQuantity() << ",";
        oss << this->getOldQuantity() << ",";
        oss << this->getPrice() << ",";
        oss << this->getOrderValidity() << ",";
        oss << OrderStatusStr[this->getOrderStatus()] << ",";
        oss << this->getOrderType() << ",";
        oss << this->getExchangeEntryTime() << ",";
        oss << this->getAccount() << ",";
        oss << this->getStrategyId() << ",";
        oss << this->getAlgoType() << ",";
        oss << this->getRemainingQty() << ",";
        oss << this->getCancelledQty() << ",";
        oss << this->getUploadFileName() << ",";
        oss << this->getUniqueStrategyIdentifier() << ",";
        oss << this->getExchangeStatus() << ",";
        oss << this->getParentOrderId() << ",";
        oss << this->getSecondaryOrderId() << ",";

        return oss.str();
    }
#ifndef FRONTEND
    void Order::setOrderStatus(FIX::OrdStatus val) {
        /* Fix field 39
                0 = New, 1 = Partially filled, 2 = Filled
                3 = Done for day, 4 = Canceled,
                6 = Pending Cancel (e.g. result of Order Cancel Request <F>)
                7 = Stopped, 8 = Rejected, 9 = Suspended
                A = Pending New B = Calculated C = Expired
                D = Accepted for bidding
        */
        switch (val) {
            case FIX::OrdStatus_NEW:
                _orderStatus = OrderStatus_CONFIRMED;
                break;
            case FIX::OrdStatus_PARTIALLY_FILLED:
                _orderStatus = OrderStatus_PARTIALLY_FILLED;
                break;
            case FIX::OrdStatus_CANCELED:
                _orderStatus = OrderStatus_CANCELED;
                break;
            case FIX::OrdStatus_REJECTED:
                _orderStatus = OrderStatus_NEW_REJECTED;
                break;
            case FIX::OrdStatus_FILLED:
                _orderStatus = OrderStatus_FILLED;
                break;
            case FIX::OrdStatus_STOPPED:
                _orderStatus = OrderStatus_STOPPED;
                break;
            case FIX::ExecType_REPLACE:
                _orderStatus = OrderStatus_REPLACED;
                break;
            default:
                LOG(WARNING) << "Unhandled Order Status";
        }
    }
#endif

}
