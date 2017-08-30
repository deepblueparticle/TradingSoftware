#include <shared/response.h>
#include <iostream>
#include <shared/commands.h>

#ifndef FRONTEND
#include <shared/easylogging++.h>
#endif
namespace Algo {

    ConnectionRsp::ConnectionRsp(const char *buf) {
        int offset = 0;
        UNSIGNED_CHARACTER tmpChar = 0;

        DESERIALIZE_8(tmpChar, setResponseType(tmpChar), buf, offset);
        DESERIALIZE_8(tmpChar, setLoginResponse(tmpChar), buf, offset);
        setPasswordSalt(strdup_internal(buf + offset));
        offset += PASSWORD_SALT_SIZE;
	UNSIGNED_LONG tmpLong1 = 0, tmpLong2 = 0;
	DESERIALIZE_64(tmpLong1, tmpLong2, setFirstStrategyId(tmpLong2), buf, offset);
        DESERIALIZE_64(tmpLong1, tmpLong2, setBackendId(tmpLong2), buf, offset);
    }

    int
    ConnectionRsp::serialize(char *buf) {
        int bytes = 0;
        UNSIGNED_CHARACTER tmpChar = 0;
        UNSIGNED_SHORT tmpShort = 0;
		UNSIGNED_LONG tmpLong = 0;

        bytes = sizeof(UNSIGNED_SHORT);  // Leave 2 bytes for packet size
        // Put category of command
        SERIALIZE_8(tmpChar, (UNSIGNED_CHARACTER) (ResponseCategory_CONNECTION), buf, bytes);  //Response Category

        SERIALIZE_8(tmpChar, getResponseType(), buf, bytes);
        SERIALIZE_8(tmpChar, getLoginResponse(), buf, bytes);
        memcpy(buf + bytes, getPasswordSalt(), PASSWORD_SALT_SIZE);
        bytes += PASSWORD_SALT_SIZE;
	SERIALIZE_64(tmpLong, getFirstStrategyId(), buf, bytes);
        SERIALIZE_64(tmpLong, getBackendId(), buf, bytes);

        UNSIGNED_SHORT dummyBytes = 0;
        // Put size as the first field after deducting 2 bytes reserved for size
        SERIALIZE_16(tmpShort, (UNSIGNED_SHORT) (bytes - sizeof(UNSIGNED_SHORT)), buf, dummyBytes);

        return bytes;
    }
#ifndef FRONTEND
    void ConnectionRsp::dump() {
        LOG(INFO) << " Dump of ConnectionRsp response ResponseType [" << (UNSIGNED_SHORT) getResponseType()
        << "] LoginResponse [" << (UNSIGNED_INTEGER) getLoginResponse() <<"] First Strategy Id [" << getFirstStrategyId() <<"]";
    }
#endif



////////////////////////////////////////////////////////////////////////////////////////

    OrderConfirmation::OrderConfirmation() {
        initialize();
    }

    void
    OrderConfirmation::initialize() {
        _clOrderId = 0;
        _exchangeOrderId = 0;
        memset(_symbolId, '0', SYMBOL_SIZE);
        _lastFillQuantity = 0;
        _lastFillPrice = 0;
        _exchangeEntryTime = 0;
        _strategyId = 0;
        _clientId = 0;
        _limitPrice = 0;
        _orderStatus = OrderStatus_MAX;
        _orderMode = OrderMode_MAX;
        _orderQuantity = 0;
        _orderPrice = 0;
        _iocCanceledQuantity = 0;
        _originalClOrderId = 0;
        _confirmationTimeSeconds = 0;
        _confirmationTimeMicroSeconds = 0;
        _isOffline = 0;
        _sequenceNumber = 0;
        _errorCode = 0;
        _reasonText = 0;
        _unknownOrder = 0;
    }

    OrderConfirmation::OrderConfirmation(const char *buf) //deserializing
    {
        int offset = 0;
        UNSIGNED_INTEGER tmp1 = 0, tmp2 = 0;
        UNSIGNED_LONG tmpLong1 = 0, tmpLong2 = 0;
        UNSIGNED_CHARACTER tmpChar = 0;

        DESERIALIZE_8(tmpChar, setOrderStatus(tmpChar), buf, offset);
        DESERIALIZE_64(tmpLong1, tmpLong2, setClOrderId(tmpLong2), buf, offset);
        DESERIALIZE_64(tmpLong1, tmpLong2, setExchangeOrderId(tmpLong2), buf, offset);

        setSymbolId(strdup_internal(buf + offset));
        offset += SYMBOL_SIZE;

        DESERIALIZE_32(tmp1, tmp2, setExchangeEntryTime(tmp2), buf, offset);
        DESERIALIZE_32(tmp1, tmp2, setStrategyId(tmp2), buf, offset);
        DESERIALIZE_32(tmp1, tmp2, setClientId(tmp2), buf, offset);
        DESERIALIZE_32(tmp1, tmp2, setLimitPrice(tmp2), buf, offset);
        DESERIALIZE_32(tmp1, tmp2, setLastFillQuantity(tmp2), buf, offset);
        DESERIALIZE_32(tmp1, tmp2, setLastFillPrice(tmp2), buf, offset);
        DESERIALIZE_8(tmpChar, setOrderMode(tmpChar), buf, offset);
        DESERIALIZE_32(tmp1, tmp2, setOrderQuantity(tmp2), buf, offset);
        DESERIALIZE_32(tmp1, tmp2, setOrderPrice(tmp2), buf, offset);
        DESERIALIZE_32(tmp1, tmp2, setIOCCanceledQuantity(tmp2), buf, offset);
        DESERIALIZE_64(tmpLong1, tmpLong2, setOriginalClOrderId(tmpLong2), buf, offset);
        DESERIALIZE_64(tmpLong1, tmpLong2, setConfirmationTimeSeconds(tmpLong2), buf, offset);
        DESERIALIZE_64(tmpLong1, tmpLong2, setConfirmationTimeMicroSeconds(tmpLong2), buf, offset);
        DESERIALIZE_8(tmpChar, setIsOffline(tmpChar), buf, offset);
        DESERIALIZE_64(tmpLong1, tmpLong2, setSequenceNumber(tmpLong2), buf, offset);
        DESERIALIZE_32(tmp1, tmp2, setErrorCode(tmp2), buf, offset);
        DESERIALIZE_32(tmp1, tmp2, setReasonText(tmp2), buf, offset);
        DESERIALIZE_8(tmpChar, setUnknownOrder(tmpChar), buf, offset);
    }

    int
    OrderConfirmation::serialize(char *buf) {
        int bytes = 0;
        UNSIGNED_SHORT tmpShort = 0;
        UNSIGNED_INTEGER tmp = 0;
        UNSIGNED_LONG tmpLong = 0;
        UNSIGNED_CHARACTER tmpChar = 0;

        bytes = sizeof(UNSIGNED_SHORT);  // Leave 2 bytes for packet size
        // Put category of command
        SERIALIZE_8(tmpChar, (UNSIGNED_CHARACTER) (ResponseCategory_ORDER_CONFIRMATION), buf, bytes);

        // Put category of command
        SERIALIZE_8(tmpChar, (UNSIGNED_CHARACTER) getOrderStatus(), buf, bytes);
        SERIALIZE_64(tmpLong, getClOrderId(), buf, bytes);
        SERIALIZE_64(tmpLong, getExchangeOrderId(), buf, bytes);

        memcpy(buf + bytes, getSymbolId(), SYMBOL_SIZE);
        bytes += SYMBOL_SIZE;


        SERIALIZE_32(tmp, getExchangeEntryTime(), buf, bytes);
        SERIALIZE_32(tmp, getStrategyId(), buf, bytes);
        SERIALIZE_32(tmp, getClientId(), buf, bytes);
        SERIALIZE_32(tmp, getLimitPrice(), buf, bytes);
        SERIALIZE_32(tmp, getLastFillQuantity(), buf, bytes);
        SERIALIZE_32(tmp, getLastFillPrice(), buf, bytes);
        SERIALIZE_8(tmpChar, (UNSIGNED_CHARACTER) getOrderMode(), buf, bytes);
        SERIALIZE_32(tmp, getOrderQuantity(), buf, bytes);
        SERIALIZE_32(tmp, getOrderPrice(), buf, bytes);
        SERIALIZE_32(tmp, getIOCCanceledQuantity(), buf, bytes);
        SERIALIZE_64(tmpLong, getOriginalClOrderId(), buf, bytes);
        SERIALIZE_64(tmpLong, getConfirmationTimeSeconds(), buf, bytes);
        SERIALIZE_64(tmpLong, getConfirmationTimeMicroSeconds(), buf, bytes);
        SERIALIZE_8(tmpChar, (UNSIGNED_CHARACTER) getIsOffline(), buf, bytes);
        SERIALIZE_64(tmpLong, getSequenceNumber(), buf, bytes);
        SERIALIZE_32(tmp, getErrorCode(), buf, bytes);
        SERIALIZE_32(tmp, getReasonText(), buf, bytes);
        SERIALIZE_8(tmpChar, (UNSIGNED_CHARACTER) getUnknownOrder(), buf, bytes);


        UNSIGNED_SHORT dummyBytes = 0;
        // Put size as the first field after deducting 2 bytes reserved for size
        SERIALIZE_16(tmpShort, (UNSIGNED_SHORT) (bytes - sizeof(UNSIGNED_SHORT)), buf, dummyBytes);

        return bytes;
    }
#ifndef FRONTEND
    void
    OrderConfirmation::dump() {
        LOG(INFO) << "getOrderStatus: " << (short) _orderStatus
        << "getClOrderId: " << getClOrderId()
        << "getExchangeOrderId: " << getExchangeOrderId()
        << "getSymbolId: " << getSymbolId()
        << "getExchangeEntryTime: " << getExchangeEntryTime()
        << "GetStrategyId: " << getStrategyId()
        << "getClientId: " << getClientId()
        << "getLimitPrice: " << getLimitPrice()
        << "getLastFillQuantity: " << getLastFillQuantity()
        << "getLastFillPrice: " << getLastFillPrice()
        << "getOrderMode: " << (short) getOrderMode()
        << "getOrderQuantity: " << getOrderQuantity()
        << "getOrderPrice: " << getOrderPrice()
        << "getIOCCanceledQuantity: " << getIOCCanceledQuantity()
        << "getOriginalClOrderId: " << getOriginalClOrderId()
        << "getConfirmationTimeSeconds: " << getConfirmationTimeSeconds()
        << "getConfirmationTimeMicroSeconds: " << getConfirmationTimeMicroSeconds()
        << "getIsOffline: " << (UNSIGNED_SHORT) getIsOffline()
        << "getSequenceNumber: " << getSequenceNumber()
        << "getErrorCode: " << getErrorCode()
        << "getReasonText: " << getReasonText()
        << "unknownOrder: " << (UNSIGNED_SHORT) getUnknownOrder();
        return;
    }
#endif
}
