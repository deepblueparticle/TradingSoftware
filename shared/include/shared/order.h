#ifndef ORDER_H
#define ORDER_H

#include <cstring>
#include <shared/commands.h>
#include <sstream>

#ifndef FRONTEND
#include <FixFields.h>
#include <fix44/ExecutionReport.h>
#endif
namespace Algo {
    class Order {

    public:
        Order();
        //Order &operator=(const Order &cSource);
        Order &operator=(Order &&other);
        Order &operator=(Order &other);

		// copy constructor
        Order(const Order &other);
        Order(Order &&other);

        TransactionType getTransactionType() const { return _transactionType; }

        const std::string& getClOrdId() const { return _clOrdId; }

        const std::string& getOrigClOrdId() const { return _origClOrdId; }

        const std::string& getExchangeOrderId() const { return _exchangeOrderId; }

        const std::string& getParentOrderId() const { return _parentOrderId; }

        const std::string& getSecondaryOrderId() const { return _secondaryOrderId; }

        const std::string& getExchangeExitTime() const { return _exchangeExitTime; }

        const std::string& getSymbol() const { return _symbol; }

        OrderMode getOrderMode() const { return _orderMode; }

        UNSIGNED_LONG getQuantity() const { return _quantity; }

        double getFilledPrice() const { return _filledPrice; }

        UNSIGNED_LONG getFilledQuantity() const { return _filledQuantity; }

        UNSIGNED_LONG getOldQuantity() const { return _oldQuantity; }

        double getPrice() const { return _price; }

        double getStopPrice() const { return _stopPrice; }

        double getAckedPrice() const { return _ackedPrice; }

        UNSIGNED_LONG getAckedQuantity() const { return _ackedQuantity; }

        TimeInForce getOrderValidity() const { return _orderValidity; }

        OrderType getOrderType() const { return _orderType; }

        OrderStatus getOrderStatus() const { return _orderStatus; }

        UNSIGNED_LONG getExchangeEntryTime() const { return _exchangeEntryTime; }

        const std::string& getAccount() const { return _account; }

        UNSIGNED_LONG getStrategyId() const { return _strategyId; }

        AlgoType getAlgoType() const { return _algoType; }

        UNSIGNED_LONG getCancelledQty() const { return _cancelledQty; }

        UNSIGNED_LONG getRemainingQty() const { return _remaining; }

        UNSIGNED_CHARACTER getExecType() const { return _execType; }

        const std::string& getExchangeStatus() const { return _exchangeStatus; }

        const std::string& getRejectReason() const { return _rejectReason; }

        const std::string& getUploadFileName() const { return _uploadFileName; }

        const std::string& getUniqueStrategyIdentifier() const { return _uniqueStrategyIdentifier; }

        void setTransactionType(TransactionType val) { _transactionType = val; }

        void setClOrdId(const std::string& val) { _clOrdId = val; }

        void setOrigClOrdId(const std::string& val) { _origClOrdId = val; }

        void setExchangeOrderId(const std::string& val) { _exchangeOrderId = val; }

        void setParentOrderId(const std::string& val) { _parentOrderId = val; }

        void setSecondaryOrderId(const std::string& val) { _secondaryOrderId = val; }

        void setExchangeExitTime(const std::string& exchangeExitTime) { _exchangeExitTime = exchangeExitTime; }

        void setSymbol(const std::string& val) { _symbol = val; }

        void setOrderMode(OrderMode val) { _orderMode = val; }

        void setQuantity(UNSIGNED_LONG val) { _quantity = val; }

        void setAckedPrice(double ackedPrice) { _ackedPrice = ackedPrice; }

        void setAckedQuantity(UNSIGNED_LONG ackedQuantity) { _ackedQuantity = ackedQuantity; }

        void setFilledPrice(double val) { _filledPrice = val; }

        void setFilledQuantity(UNSIGNED_LONG val) { _filledQuantity = val; }

        void setOldQuantity(UNSIGNED_LONG val) { _oldQuantity = val; }

        void setPrice(double val) { _price = val; }

        void setStopPrice(double val) { _stopPrice = val; }

        void setOrderValidity(TimeInForce val) { _orderValidity = val; }

        void setOrderType(OrderType val) { _orderType = val; }

        void setOrderStatus(OrderStatus val) { _orderStatus = val; }

        void setExchangeStatus(const std::string& val) { _exchangeStatus = val; }
        
        void setExchangeEntryTime(UNSIGNED_LONG val) { _exchangeEntryTime = val; }

        void setAccount(const std::string& val) { _account = val; }

        void setStrategyId(UNSIGNED_LONG id) { _strategyId = id; }

        void setAlgoType(AlgoType algoType) { _algoType = algoType; }

        void setCancelledQty(UNSIGNED_LONG qty) { _cancelledQty = qty; }

        void setRemaining(UNSIGNED_LONG qty) { _remaining = qty; }

        void setExecType(UNSIGNED_CHARACTER val) { _execType = val; }

        void setRejectReason(const std::string& reason) { _rejectReason = reason; }

        void setUploadFileName(const std::string& name) { _uploadFileName = name; }

        void setUniqueStrategyIdentifier(const std::string& identifier) { _uniqueStrategyIdentifier = identifier; }

        void initialize();

        void dumpOrder();

        std::string toString() const;

        static bool fromString(const std::string& str, Order& order);
#ifndef FRONTEND
        void setOrderStatus(FIX::OrdStatus val);
#endif
    private:
        TransactionType _transactionType;
        std::string _clOrdId;
        std::string _origClOrdId;
        std::string _exchangeOrderId;
        std::string _parentOrderId;
        std::string _secondaryOrderId;
        std::string _symbol;
        OrderMode _orderMode;
        UNSIGNED_LONG _quantity;
        UNSIGNED_LONG _disclosedQuantity;
        double _filledPrice;
        UNSIGNED_LONG _filledQuantity;
        UNSIGNED_LONG _oldQuantity;
        UNSIGNED_LONG _ackedQuantity;
        double _ackedPrice;
        double _price;
        double _stopPrice;
        TimeInForce _orderValidity;
        OrderType _orderType;
        OrderStatus _orderStatus;
        UNSIGNED_LONG _exchangeEntryTime;
        std::string _account;
        UNSIGNED_LONG _strategyId;
        AlgoType _algoType;

        UNSIGNED_LONG _cancelledQty;
        UNSIGNED_LONG _remaining;
        UNSIGNED_CHARACTER _execType;
        std::string _rejectReason;
        std::string _uploadFileName;
        std::string _uniqueStrategyIdentifier;
        std::string _exchangeExitTime;
        std::string _exchangeStatus;

    };
}
#endif // ORDER_H
