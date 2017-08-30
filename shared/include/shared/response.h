#ifndef RESPONSE
#define RESPONSE

#include "defines.h"

namespace Algo {
	enum LoginResponse 
        {
          LoginResponse_SUCCESS,
          LoginResponse_BAD_PASSWORD,
          LoginResponse_MULTIPLE_LOGINS,
          LoginResponse_MAX
	};

	enum ResponseCategory {
		ResponseCategory_CONNECTION,
		ResponseCategory_STRATEGY,
		ResponseCategory_ORDER_CONFIRMATION,
		ResponseCategory_MAX
	};

	// CONNECTION_RESPONSE
	class ConnectionRsp {
	public:
		ConnectionRsp() { }

		ConnectionRsp(const char *buf);

		int serialize(char *buf);
#ifndef FRONTEND
		void dump();
#endif
		UNSIGNED_CHARACTER getResponseType() const { return _responseType; }
		UNSIGNED_CHARACTER getLoginResponse() const { return _loginResponse; }
		char *getPasswordSalt() { return _passwordSalt; }
		UNSIGNED_LONG getFirstStrategyId() { return _firstStrategyId;  }
                UNSIGNED_LONG getBackendId() { return _backendId; }

		void setResponseType(UNSIGNED_CHARACTER responseType) { _responseType = responseType; }
		void setLoginResponse(UNSIGNED_CHARACTER loginResponse) { _loginResponse = loginResponse; }
		void setPasswordSalt(char *passwordSalt) { memcpy(_passwordSalt, passwordSalt, PASSWORD_SALT_SIZE); }
		void setFirstStrategyId(UNSIGNED_LONG id) { _firstStrategyId = id;  }
                void setBackendId(UNSIGNED_LONG id) { _backendId = id; }

	private:
		UNSIGNED_CHARACTER _responseType;
		UNSIGNED_CHARACTER _loginResponse;
		char _passwordSalt[PASSWORD_SALT_SIZE];
		UNSIGNED_LONG      _firstStrategyId;
                UNSIGNED_LONG      _backendId;
	};

	/////////////////////////////////////////////////////////////////////////
	class OrderConfirmation {
	public:
		OrderConfirmation();

		OrderConfirmation(const char *buf);

		void initialize();

		UNSIGNED_LONG getClOrderId() const { return _clOrderId; }

		UNSIGNED_LONG getExchangeOrderId() const { return _exchangeOrderId; }

		char *getSymbolId() { return _symbolId; }

		UNSIGNED_INTEGER getLastFillQuantity() const { return _lastFillQuantity; }

		UNSIGNED_INTEGER getLastFillPrice() const { return _lastFillPrice; }

		UNSIGNED_INTEGER getExchangeEntryTime() const { return _exchangeEntryTime; }

		UNSIGNED_INTEGER getStrategyId() const { return _strategyId; }

		UNSIGNED_INTEGER getClientId() const { return _clientId; }

		UNSIGNED_INTEGER getLimitPrice() const { return _limitPrice; }

		UNSIGNED_CHARACTER getOrderStatus() const { return _orderStatus; }

		UNSIGNED_CHARACTER getOrderMode() const { return _orderMode; }

		UNSIGNED_INTEGER getOrderQuantity() const { return _orderQuantity; }

		UNSIGNED_INTEGER getOrderPrice() const { return _orderPrice; }

		UNSIGNED_INTEGER getIOCCanceledQuantity() const { return _iocCanceledQuantity; }

		UNSIGNED_LONG getOriginalClOrderId() const { return _originalClOrderId; }

		UNSIGNED_LONG getConfirmationTimeSeconds() const { return _confirmationTimeSeconds; }

		UNSIGNED_LONG getConfirmationTimeMicroSeconds() const { return _confirmationTimeMicroSeconds; }

		UNSIGNED_CHARACTER getIsOffline() const { return _isOffline; }

		UNSIGNED_LONG getSequenceNumber() const { return _sequenceNumber; }

		UNSIGNED_INTEGER getErrorCode() const { return _errorCode; }

		UNSIGNED_INTEGER getReasonText() const { return _reasonText; }

		UNSIGNED_CHARACTER getUnknownOrder() const { return _unknownOrder; }


		void setClOrderId(UNSIGNED_LONG clOrderId) { _clOrderId = clOrderId; }

		void setExchangeOrderId(UNSIGNED_LONG exchangeOrderId) { _exchangeOrderId = exchangeOrderId; }

		void setSymbolId(const char *symbolId) { strcpy(_symbolId, symbolId); }

		void setLastFillQuantity(UNSIGNED_INTEGER qty) { _lastFillQuantity = qty; }

		void setLastFillPrice(UNSIGNED_INTEGER price) { _lastFillPrice = price; }

		void setExchangeEntryTime(UNSIGNED_INTEGER exchangeEntryTime) { _exchangeEntryTime = exchangeEntryTime; }

		void setStrategyId(UNSIGNED_INTEGER strategyId) { _strategyId = strategyId; }

		void setClientId(UNSIGNED_INTEGER clientId) { _clientId = clientId; }

		void setLimitPrice(UNSIGNED_INTEGER limitPrice) { _limitPrice = limitPrice; }

		void setOrderStatus(UNSIGNED_CHARACTER orderStatus) { _orderStatus = orderStatus; }

		void setOrderMode(UNSIGNED_CHARACTER orderMode) { _orderMode = orderMode; }

		void setOrderQuantity(UNSIGNED_INTEGER quantity) { _orderQuantity = quantity; }

		void setOrderPrice(UNSIGNED_INTEGER price) { _orderPrice = price; }

		void setIOCCanceledQuantity(UNSIGNED_INTEGER quantity) { _iocCanceledQuantity = quantity; }

		void setOriginalClOrderId(UNSIGNED_LONG originalClOrderId) { _originalClOrderId = originalClOrderId; }

		void setConfirmationTimeSeconds(UNSIGNED_LONG seconds) { _confirmationTimeSeconds = seconds; }

		void setConfirmationTimeMicroSeconds(UNSIGNED_LONG microSeconds) { _confirmationTimeMicroSeconds = microSeconds; }

		void setIsOffline(UNSIGNED_CHARACTER isOffline) { _isOffline = isOffline; }

		void setSequenceNumber(UNSIGNED_LONG sequenceNumber) { _sequenceNumber = sequenceNumber; }

		void setErrorCode(UNSIGNED_INTEGER errorCode) { _errorCode = errorCode; }

		void setReasonText(UNSIGNED_INTEGER reasonText) { _reasonText = reasonText; }

		void setUnknownOrder(UNSIGNED_CHARACTER unknownOrder) { _unknownOrder = unknownOrder; }
#ifndef FRONTEND
		void dump();
#endif
		int serialize(char *buf);

	private:
		UNSIGNED_LONG _clOrderId;
		UNSIGNED_LONG _exchangeOrderId;
		char _symbolId[SYMBOL_SIZE];
		UNSIGNED_INTEGER _lastFillQuantity;
		UNSIGNED_INTEGER _lastFillPrice;
		UNSIGNED_INTEGER _exchangeEntryTime;
		UNSIGNED_INTEGER _strategyId;
		UNSIGNED_INTEGER _clientId;
		// Limit price will come in Market to Limit order conversion
		UNSIGNED_INTEGER _limitPrice;
		UNSIGNED_CHARACTER _orderStatus;
		UNSIGNED_CHARACTER _orderMode;
		// Adding following two fields for market making
		UNSIGNED_INTEGER _orderQuantity;
		UNSIGNED_INTEGER _orderPrice;
		UNSIGNED_INTEGER _iocCanceledQuantity;
		UNSIGNED_LONG _originalClOrderId;
		UNSIGNED_LONG _confirmationTimeSeconds;
		UNSIGNED_LONG _confirmationTimeMicroSeconds;
		UNSIGNED_CHARACTER _isOffline;
		UNSIGNED_LONG _sequenceNumber;
		UNSIGNED_INTEGER _errorCode;
		UNSIGNED_INTEGER _reasonText;
		UNSIGNED_CHARACTER _unknownOrder;
	};

}
#endif // RESPONSE


