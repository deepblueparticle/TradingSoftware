/*
 * File:   commands.h
 * Author: Abhishek Anand
 *
 * Created on May 8, 2015, 8:53 PM
 */

#ifndef COMMANDS_H
#define COMMANDS_H

#include "defines.h"
#include <boost/serialization/access.hpp>
#include <cassert>

namespace Algo {
    enum ComparationType {
        GREATER,
        LESS
    };

    enum OrderMode {
        OrderMode_BUY=1,
        OrderMode_SELL,
        OrderMode_MAX
    };


    const std::string OrderModeStr[] = {
            "EMPTY",
            "BUY",
            "SELL",
            "MAX"};

    enum OrderStatus {
        OrderStatus_PENDING, // = 0
        OrderStatus_CONFIRMED,
        OrderStatus_FILLED, // Same for partial fill and fill
        OrderStatus_CANCELED,
        OrderStatus_STOPPED,
        OrderStatus_REPLACED,
        OrderStatus_NEW_REJECTED,
        OrderStatus_CANCEL_REJECTED,
        OrderStatus_REPLACE_REJECTED,
        OrderStatus_FROZEN,
        OrderStatus_MARKET_TO_LIMIT,
        OrderStatus_TRIGGERED,
        OrderStatus_PARTIALLY_FILLED,
        OrderStatus_CANCELED_OF_IOC,
        OrderStatus_MAX
    };

    const std::string OrderStatusStr[] = {
            "pending",
            "confirmed",
            "filled",
            "cancelled",
            "stopped",
            "replaced",
            "new rejected",
            "cancel rejected",
            "replace rejected",
            "frozen",
            "market_to_limit",
            "triggered",
            "partially filled",
            "cancelLed of ioc",
            "max"};

    enum TransactionType {
        TransactionType_NEW,
        TransactionType_MODIFY,
        TransactionType_CANCEL,
        TransactionType_MAX
    };

    enum AlertType {
        AlertType_EXEC_REPORT,
        AlertType_MSG,
        AlertType_MAX
    };

    enum TimeInForce {
        TimeInForce_GTC,
        TimeInForce_IOC,
        TimeInForce_MAX
    };

    enum OrderType {
        OrderType_LIMIT,
        OrderType_MARKET,
        OrderType_STOP_LIMIT,
        OrderType_STOP,
        OrderType_TRAIL_STOP,
        OrderType_MAX
    };


    enum CommandCategory {
        CommandCategory_LOGIN,                      // First command
        CommandCategory_LOGOUT,                     // Logout command
        CommandCategory_GET_TOTAL_RECORDS_SCRIP_MASTER,
        CommandCategory_SEND_SCRIP_MASTER_DATA,
        CommandCategory_SEND_SCRIP_MASTER_DATA_DONE,
        CommandCategory_ALGO_MODEL_1,
        CommandCategory_CONNECTION,
        CommandCategory_RESYNC_ALGO,
        CommandCategory_RESYNC_ALGO_MISSING_ID,
        CommandCategory_RESYNC_ALGO_DONE,
        CommandCategory_ALERT,
        CommandCategory_ORDERRESYNC_REQ,
        CommandCategory_ORDERRESYNC_RESP,
        CommandCategory_ORDERRESYNC_DONE,
        CommandCategory_RESYNC_ALERTS_DONE,
        CommandCategory_STRATEGY_ID,
        CommandCategory_STOP_STRATEGY
    };


    enum StrategyStatus {
        StrategyStatus_PENDING=1,
        StrategyStatus_WAITING,
        StrategyStatus_OPEN,
        StrategyStatus_STOPPED,
        StrategyStatus_SARTED,
        StrategyStatus_RUNNING,
        StrategyStatus_UNKNOWN,
        StrategyStatus_START,
        StrategyStatus_MAX
    };

    enum AlgoType {
        Algo_1 = 1,
        Algo_2,
        Algo_MAX
    };


    const std::string AlgoTypeStr[] = {
            "",
            "Algo_1",
            "Algo_2",
            "MAX"};

////////////////////////////////////////////////////////////////////////
// Login Command
    class Login {
    public:
        Login(const char username[],
              const char isSaltRequest,
              const char password[],
              UNSIGNED_INTEGER t);

        Login(const char *buf);
        int Serialize(char *buf);
        const char *GetUserName() { return _userName; }
        char *GetPassword() { return _password; }
        UNSIGNED_CHARACTER IsSaltRequest() { return _isSaltRequest; }
        void SetHeartbeatTimeout(UNSIGNED_INTEGER t) { _heartbeatTimeout = t; }

        void SetUserName(const char *str) {
          STRCPY_ASSERT(_userName, str, USERNAME_SIZE);
        }

        void SetSaltRequest(UNSIGNED_CHARACTER value) {
          _isSaltRequest = value;
        }

        void SetPassword(const char *password) {
          STRCPY_ASSERT(_password, password, PASSWORD_SIZE);
        }

        UNSIGNED_INTEGER GetHeartbeatTimeout() { return _heartbeatTimeout; }
        void Dump(std::string &str);
#ifndef FRONTEND
        void Dump();
#endif
    private:
        char _userName[USERNAME_SIZE];
        UNSIGNED_CHARACTER _isSaltRequest;
        char _password[PASSWORD_SIZE];
        UNSIGNED_INTEGER _heartbeatTimeout;
    };


    class ScripMasterDataRequest {
    public:
        ScripMasterDataRequest() { }
        ScripMasterDataRequest(const char *buf);
        int Serialize(char *buf);
        void SetSymbol(const char *symbol) {
          STRCPY_ASSERT(_symbol, symbol, SYMBOL_SIZE);
        }
        void SetTickSize(float tickSize) { _tickSize = tickSize; }
        void SetLotSize(UNSIGNED_LONG lotSize) { _lotSize = lotSize; }
        void SetPrecission(UNSIGNED_INTEGER p) { _precission = p; }


        char *GetSymbol() { return _symbol; }
        float GetTickSize() { return _tickSize; }
        UNSIGNED_LONG GetLotSize() { return _lotSize; }
        UNSIGNED_INTEGER GetPrecission() { return _precission; }
#ifndef FRONTEND
        void dump();
#endif
    private:
        char _symbol[SYMBOL_SIZE];
        float _tickSize;
        UNSIGNED_LONG _lotSize;
        UNSIGNED_INTEGER _precission;
    };

    class AlgoModelStrategyId {
    public:
       AlgoModelStrategyId() { }
       AlgoModelStrategyId(const char *buf);
       int serialize(char *buf);
       void setStrategyId(UNSIGNED_LONG sid) { _strategyId = sid; }
       void setInternalId(UNSIGNED_LONG iid) { _internalId = iid; }
       UNSIGNED_LONG getStrategyId() const { return _strategyId; }
       UNSIGNED_LONG getInternalId() const { return _internalId; }

    private:
        UNSIGNED_LONG _strategyId;
        UNSIGNED_LONG _internalId;
    };

    class AlgoModelStopStrategy {
    public:
       AlgoModelStopStrategy() { }
       AlgoModelStopStrategy(const char *buf);
       int serialize(char *buf);
       void setStrategyId(UNSIGNED_LONG sid) { _strategyId = sid; }
       UNSIGNED_LONG getStrategyId() const { return _strategyId; }

    private:
        UNSIGNED_LONG _strategyId;
    };

    class AlgoModel1 {
    public:
        AlgoModel1() { }

        AlgoModel1(const char *buf);
        int Serialize(char *buf);
        void SetStrategyId(UNSIGNED_LONG sid) { _strategyId = sid; }
        void SetInternalId(UNSIGNED_LONG iid) { _internalId = iid; }

        void SetTickSize(float tickSize) { _tickSize = tickSize; }
        void SetMode(UNSIGNED_CHARACTER mode) { _mode = mode; }
        void SetPrevClose(double pclose) { _prevClose = pclose; }
        void SetBaseValue(double bval) { _baseVal = bval; }
        void SetEntryPrice(double eprice) { _entryPrice = eprice; }
        void SetEntryPriceCalc(double eprice) { _entryPriceCalculated = eprice; }
        void SetPointA(double pa) { _pointAPercentage = pa; }
        void SetPointACalc(double pa) { _pointACalculated = pa; }
        void SetPointB(double pb) { _pointBPercentage = pb; }
        void SetPointBCalc(double pb) { _pointBCalculated = pb; }
        void SetCancelPoint1(double cp) { _cp1Percentage = cp; }
        void SetCancelPoint1Calc(double cp) { _cp1Calculated = cp; }
        void SetCancelPoint2(double cp) { _cp2Percentage = cp; }
        void SetCancelPoint2Calc(double cp) { _cp2Calculated = cp; }
        void setParentOrderPrice(double parentOrderPrice_) { _parentOrderPrice = parentOrderPrice_; }
        void SetProfit(double pp) { _profitPercentage = pp; }
        void SetProfitCalc(double pp) { _profitCalculated = pp; }
        void SetLoss(double lp) { _lossPercentage = lp; }
        void SetLossCalc(double lp) { _lossCalculated = lp; }
        void SetLossLimitPrice(double lossLimitPrice_) { _lossLimitPrice = lossLimitPrice_; }
        void SetTolerance(double tol) { _tolerance = tol; }
        void SetTimeEnabled(UNSIGNED_INTEGER e) { _timeEnabled = e; }

        void SetStartTime(const char* stime) {
          STRCPY_ASSERT(_startTime, stime, TIME_SIZE);
        }

        void SetEndTime(const char* etime) {
          STRCPY_ASSERT(_endTime, etime, TIME_SIZE);
        }
        void SetStartDate(const char* stime) {
          STRCPY_ASSERT(_startDate, stime, TIME_SIZE);
        }

        void SetEndDate(const char* etime) {
          STRCPY_ASSERT(_endDate, etime, TIME_SIZE);
        }

        void SetStartTimeEpoch(UNSIGNED_LONG stime) { _startTimeEpoch = stime; }
        void SetEndTimeEpoch(UNSIGNED_LONG etime) { _endTimeEpoch = etime; }
        void SetStartDateEpoch(UNSIGNED_LONG stime) { _startDateEpoch = stime; }
        void SetEndDateEpoch(UNSIGNED_LONG etime) { _endDateEpoch = etime; }

        void setSecondaryOrderExitDateEpoch(UNSIGNED_LONG edate) {
          m_secondaryOrderExitDateEpoch = edate;
        }

        void setSecondaryOrderExitTimeEpoch(UNSIGNED_LONG etime) {
            m_secondaryOrderExitTimeEpoch = etime;
        }

        void setSecondaryOrderExitDate(const char *date) {
          STRCPY_ASSERT(m_secondaryOrderExitDate, date, TIME_SIZE);
        }

        void setSecondaryOrderExitTime(const char *time) {
          STRCPY_ASSERT(m_secondaryOrderExitTime, time, TIME_SIZE);
        }

        void setSecondaryOrderExitEnabled(bool enaled) {
          m_secondaryOrderExitEnabled = enaled;
        }

        void SetStatus(UNSIGNED_CHARACTER stat) { _status = stat; }
        void SetQty(UNSIGNED_LONG qty) { _qty = qty; }
        void SetMaxQty(UNSIGNED_LONG maxQty) { _maxQty = maxQty; }

        void SetPointATime(const char *time) {
          STRCPY_ASSERT(_pointATime, time, TIME_SIZE);
        }

        void SetPointBTime(const char *time) {
          STRCPY_ASSERT(_pointBTime, time, TIME_SIZE);
        }

        void SetCancelTime(const char *time) {
          STRCPY_ASSERT(_cancelTime, time, TIME_SIZE);
        }

        void SetCancelTicks(UNSIGNED_LONG ticks) { _cancelTicks = ticks; }

        void SetCPATime(const char *time) {
          STRCPY_ASSERT(_cpATime, time, TIME_SIZE);
        }

        void SetCPBTime(const char* time) {
          STRCPY_ASSERT(_cpBTime, time, TIME_SIZE);
        }

        void SetEntryPriceTime(const char *time) {
          STRCPY_ASSERT(_entryPriceTime, time, TIME_SIZE);
        }

        void SetStopTime(const char *time) {
          STRCPY_ASSERT(_stopTime, time, TIME_SIZE);
        }

        void SetLimitTime(const char *time) {
          STRCPY_ASSERT(_limitTime, time, TIME_SIZE);
        }

        void SetTrailing(double t) { _trailing = t; }
        void SetTrailingPercentage(double trailingPercentage) { _trailingPercentage = trailingPercentage; }
        void SetAlgoType(UNSIGNED_CHARACTER type) { _algoType = type; }

        void SetUserName(const char *name) {
          STRCPY_ASSERT(_userName, name, TIME_SIZE);
        }

        void SetDailyOrWeekly(const char* type) {
          STRNCPY_ASSERT(_dailyOrWeekly, type, DAILY_WEEKLY_SIZE);
        }

        void SetIdentifier(const char* identifier) {
          STRNCPY_ASSERT(_identifier, identifier, IDENTIFIER_SIZE);
        }

        void SetUpname(const char* upname) {
          STRNCPY_ASSERT(_upname, upname, UPLOAD_FILE_NAME_SIZE);
        }

        void SetDataSymbol(const char *dataSymbol) {
            STRNCPY_ASSERT(_dataSymbol, dataSymbol, SYMBOL_SIZE);
        }
        void SetDataSecType(const char *dataSecType) {
            STRNCPY_ASSERT(_dataSecType, dataSecType, SYMBOL_SIZE);
        }
        void SetDataExchange(const char *dataExchange) {
            STRNCPY_ASSERT(_dataExchange, dataExchange, SYMBOL_SIZE);
        }
        void SetDataCurrency(const char *dataCurrency) {
            STRNCPY_ASSERT(_dataCurrency, dataCurrency, SYMBOL_SIZE);
        }

        UNSIGNED_LONG GetStrategyId() const { return _strategyId; }
        UNSIGNED_LONG GetInternalId() const { return _internalId; }
        float GetTickSize() const { return _tickSize; }
        UNSIGNED_CHARACTER GetMode() const { return _mode; }
        double GetPrevClose() const { return _prevClose; }
        double GetBaseValue() const { return _baseVal; }
        double GetEntryPrice() const { return _entryPrice; }
        double GetEntryPriceCalc() const { return _entryPriceCalculated; }
        double GetPointA() const { return _pointAPercentage; }
        double GetPointACalc() const { return _pointACalculated; }
        double GetPointB() const { return _pointBPercentage; }
        double GetPointBCalc() const { return _pointBCalculated; }
        double GetCancelPoint1() const { return _cp1Percentage; }
        double GetCancelPoint1Calc() const { return _cp1Calculated; }
        double GetCancelPoint2() const { return _cp2Percentage; }
        double GetCancelPoint2Calc() const { return _cp2Calculated; }
        double getParentOrderPrice() const { return _parentOrderPrice; }
        double GetProfit() const { return _profitPercentage; }
        double GetProfitCalc() const { return _profitCalculated; }
        double GetLoss() const { return _lossPercentage; }
        double GetLossCalc() const { return _lossCalculated; }
        double GetLossLimitPrice() const { return _lossLimitPrice; }
        double GetTolerance() const { return _tolerance; }
        UNSIGNED_INTEGER GetIsTimeEnabled() const { return _timeEnabled; }
        const char *GetStartTime() const { return _startTime; }
        const char *GetEndTime() const { return _endTime; }
        const char *GetStartDate() const { return _startDate; }
        const char *GetEndDate() const { return _endDate; }
        UNSIGNED_LONG GetStartTimeEpoch() const { return _startTimeEpoch; }
        UNSIGNED_LONG GetEndTimeEpoch() const { return _endTimeEpoch; }
        UNSIGNED_LONG GetStartDateEpoch() const { return _startDateEpoch; }
        UNSIGNED_LONG GetEndDateEpoch() const { return _endDateEpoch; }
        double getTrailingPercentage() { return _trailingPercentage; }

        UNSIGNED_LONG getSecondaryOrderExitDateEpoch() const {
          return m_secondaryOrderExitDateEpoch;
        }

        UNSIGNED_LONG getSecondaryOrderExitTimeEpoch() const {
          return m_secondaryOrderExitTimeEpoch;
        }
        std::string getSecondaryOrderExitDate() const {
          return std::string(m_secondaryOrderExitDate);
        }
        std::string getSecondaryOrderExitTime() const {
          return std::string(m_secondaryOrderExitTime);
        }
        bool getSecondaryOrderExitEnabled() const {
          return m_secondaryOrderExitEnabled;
        }

        UNSIGNED_CHARACTER GetStatus() const { return _status; }
        UNSIGNED_LONG GetQty() const { return _qty; }
        UNSIGNED_LONG GetMaxQty() const { return _maxQty; }
        const char *GetPointATime() const { return _pointATime; }
        const char *GetPointBTime() const { return _pointBTime; }
        const char *GetCancelTime() const { return _cancelTime; }
        const char *GetEntryPriceTime() const { return _entryPriceTime; }
        const char *GetStopTime() const { return _stopTime; }
        const char *GetLimitTime() const { return _limitTime; }

        UNSIGNED_LONG GetCancelTicks() const { return _cancelTicks; }
        const char *GetCPATime() const { return _cpATime; }
        const char *GetCPBTime() const { return _cpBTime; }
        double GetTrailingStop() const { return _trailing; }
        UNSIGNED_CHARACTER GetAlgoType() const { return _algoType; }
        const char *GetUserName() const { return _userName; }
        const char *GetDailyOrWeekly()const { return _dailyOrWeekly; }
        const char *GetIdentifier() const { return _identifier; }
        const char *GetUpname() { return _upname; }

        const char* GetDataSymbol() { return _dataSymbol; }
        const char* GetDataSecType() { return _dataSecType; }
        const char* GetDataExchange() { return _dataExchange; }
        const char* GetDataCurrency() { return _dataCurrency; }
#ifndef FRONTEND
        void Dump(std::string prefix);
#endif
#ifdef FRONTEND
       void DumpToCsv(std::string, double pnl=0, double avgBuyPrice=0, double avgSellPrice=0);
       void Dump();
#endif
    private:
        UNSIGNED_LONG _strategyId;
        UNSIGNED_LONG _internalId;
        float _tickSize;
        UNSIGNED_CHARACTER _mode;
        double _prevClose;
        double _baseVal;
        double _entryPrice;
        double _entryPriceCalculated;
        double _pointAPercentage;
        double _pointACalculated;
        double _pointBPercentage;
        double _pointBCalculated;
        double _cp1Percentage;
        double _cp1Calculated;
        double _cp2Percentage;
        double _cp2Calculated;
        double _parentOrderPrice;
        double _profitPercentage;
        double _profitCalculated;
        double _lossLimitPrice;
        double _lossPercentage;
        double _lossCalculated;
        double _tolerance;
        UNSIGNED_INTEGER _timeEnabled;
        char _startTime[TIME_SIZE];
        char _endTime[TIME_SIZE];
        char _startDate[TIME_SIZE];
        char _endDate[TIME_SIZE];
        UNSIGNED_CHARACTER _status;
        UNSIGNED_LONG _qty;
        UNSIGNED_LONG _maxQty;
        char _pointATime[TIME_SIZE];
        char _pointBTime[TIME_SIZE];
        char _cancelTime[TIME_SIZE];
        UNSIGNED_LONG _cancelTicks;
        char _cpATime[TIME_SIZE];
        char _cpBTime[TIME_SIZE];

        char _entryPriceTime[TIME_SIZE];
        char _stopTime[TIME_SIZE];
        char _limitTime[TIME_SIZE];

        double _trailing;
        double _trailingPercentage;
        UNSIGNED_CHARACTER _algoType;

        UNSIGNED_LONG _startTimeEpoch;
        UNSIGNED_LONG _endTimeEpoch;
        UNSIGNED_LONG _startDateEpoch;
        UNSIGNED_LONG _endDateEpoch;

        // adding T-exit parameter
        UNSIGNED_LONG m_secondaryOrderExitDateEpoch;
        UNSIGNED_LONG m_secondaryOrderExitTimeEpoch;
        char m_secondaryOrderExitDate[TIME_SIZE];
        char m_secondaryOrderExitTime[TIME_SIZE];
        bool m_secondaryOrderExitEnabled;

        char _userName[CLIENT_NAME_SIZE];
        char _dailyOrWeekly[DAILY_WEEKLY_SIZE];
        char _upname[UPLOAD_FILE_NAME_SIZE];
        char _identifier[IDENTIFIER_SIZE];

        char _dataSymbol[SYMBOL_SIZE];
        char _dataSecType[SYMBOL_SIZE];
        char _dataExchange[SYMBOL_SIZE];
        char _dataCurrency[SYMBOL_SIZE];
    };


    class Heartbeat {
    public:
        Heartbeat(UNSIGNED_CHARACTER feed, UNSIGNED_CHARACTER op);
        Heartbeat(const char *buf);
        int Serialize(char *buf);
#ifndef FRONTEND
        void Dump();
#endif
        UNSIGNED_CHARACTER GetFeedConnection() { return _isFeedConnected; }
        UNSIGNED_CHARACTER GetOPConnection() { return _isOPConnected; }
        void SetFeedConnection(UNSIGNED_CHARACTER value) { _isFeedConnected = value; }
        void SetOPConnection(UNSIGNED_CHARACTER value) { _isOPConnected = value; }


    private:
        UNSIGNED_CHARACTER _isFeedConnected;
        UNSIGNED_CHARACTER _isOPConnected;

    };

    class Alerts {
    public:
        Alerts() { }

        Alerts(const char *buf);

        int Serialize(char *buf);
        void SetStrategyId(UNSIGNED_LONG sid) { _strategyId = sid; }
        void SetInternalId(UNSIGNED_LONG iid) { _internalId = iid; }
        void SetTimeStamp(UNSIGNED_LONG t) { _timeStamp = t; }

        void SetReason(const char *reason) {
          assert(ALERT_MSG_SIZE > strlen(reason));
          strcpy(_reason, reason);
        }

        UNSIGNED_LONG GetStrategyId() const { return _strategyId; }
        UNSIGNED_LONG GetInternalId() const { return _internalId; }
        UNSIGNED_LONG GetTimeStamp() const { return _timeStamp; }
        const char *GetReason() const { return _reason; }
        UNSIGNED_CHARACTER GetAlertType() const { return _alertType; }
        void SetAlertType(UNSIGNED_CHARACTER at) { _alertType = static_cast<AlertType>(at); }

        void SetOrderStr(const char *str) {
          assert(MAX_BUF > strlen(str));
          strcpy(_orderStr, str);
        }

        char *GetOrderStr() { return _orderStr; }
#ifndef FRONTEND
        void Dump(std::string prefix);
#else
        void Dump(std::string filename);
#endif

    private:
        UNSIGNED_LONG _strategyId;
        UNSIGNED_LONG _internalId;
        UNSIGNED_LONG _timeStamp;

        UNSIGNED_CHARACTER _alertType;
        char _orderStr[MAX_BUF];
        char _reason[ALERT_MSG_SIZE];
    };

    class OrderResync {
    public:
        OrderResync() { }

        OrderResync(const char *buf, bool req = true);
        int Serialize(char *buf, bool req = true);
        UNSIGNED_LONG GetLastIndex() { return _lastIndex;  }
        void SetLastIndex(UNSIGNED_LONG index) { _lastIndex = index; }

        void SetOrderStr(const char *str) {
          assert(MAX_BUF > strlen(str));
          strcpy(_orderStr, str);
        }

        char *GetOrderStr() { return _orderStr; }
#ifndef FRONTEND
        void Dump(std::string prefix);
#endif
    private:
        UNSIGNED_LONG  _lastIndex;
        char _orderStr[MAX_BUF];
    };

  class AlgoResync {
  public:
    AlgoResync() { }

    AlgoResync(const char *buf);
    int Serialize(char *buf);
    UNSIGNED_LONG GetLastIndex() { return _lastIndex; }
    void SetLastIndex(UNSIGNED_LONG index) { _lastIndex = index; }

  private:
    UNSIGNED_LONG  _lastIndex;
  };

  class AlertResync {
  public:
    AlertResync() { }

    AlertResync(const char *buf);
    int Serialize(char *buf);
    UNSIGNED_LONG GetLastIndex() { return _lastIndex; }
    void SetLastIndex(UNSIGNED_LONG index) { _lastIndex = index; }

  private:
    UNSIGNED_LONG  _lastIndex;
  };

    class ConnStatus
    {
    public:
        ConnStatus(){}
        int Serialize(char* buf);
    };


}
#endif // COMMANDS_H

