#include <shared/commands.h>
#include <iostream>
#include <sstream>
#include <string>
#ifndef FRONTEND
#include <shared/easylogging++.h>
#endif
#ifdef FRONTEND
#include <fstream>
#include <QDebug>
#include <QDateTime>
#include <QByteArray>
#include <QTimeZone>
#endif

namespace  Algo {
/////////////////////////////////////////////////////////////////

    Login::Login(const char userName[],
                 const char isSaltRequest,
                 const char password[],
                 UNSIGNED_INTEGER h) {
        strcpy(_userName, userName);
        _isSaltRequest = isSaltRequest;
        strcpy(_password, password);
        _heartbeatTimeout = h;
    }

    Login::Login(const char *buf) //deserializing
    {
        int offset = 0;
        UNSIGNED_CHARACTER tmpChar = 0;
        UNSIGNED_INTEGER tmpInt1 = 0, tmpInt2 = 0;

        SetUserName(strdup_internal(buf + offset));
        offset += USERNAME_SIZE;

        DESERIALIZE_8(tmpChar, SetSaltRequest(tmpChar), buf, offset);
        SetPassword(strdup_internal(buf + offset));
        offset += PASSWORD_SIZE;
        DESERIALIZE_32(tmpInt1, tmpInt2, SetHeartbeatTimeout(tmpInt2), buf, offset);
    }
#ifndef FRONTEND
    void Login::Dump() {
        LOG(INFO) << "Login request: "
        << "UserName: " << std::string(_userName, USERNAME_SIZE) << " "
        << "Password: " << _password << " "
        << "Is Salt Req: " << (int) _isSaltRequest << " "
        << "Heartbeat timeout: " << _heartbeatTimeout;
    }
#endif

    void Login::Dump(std::string & str) {
        str += std::string("Login request: UserName: ")
               + std::string(_userName);
        str += std::string(" Password: ")
               + std::string((char *) _password);
        str += std::string(" SaltRequest: ")
               + std::to_string((int)_isSaltRequest);
        str += std::string(" HeartbeatTimeout: ")
               + std::to_string(_heartbeatTimeout)
               + std::string("\n");
    }


    int Login::Serialize(char *buf) {
        int bytes = 0;
        UNSIGNED_CHARACTER tmpChar;
        UNSIGNED_SHORT tmpShort = 0;
        UNSIGNED_INTEGER tmpInt = 0;

        bytes = sizeof(UNSIGNED_SHORT);  // Leave 2 bytes for packet size

        // Put category of command
        SERIALIZE_8(tmpChar, (UNSIGNED_CHARACTER) (CommandCategory_LOGIN), buf, bytes);  //Command Category

        // Put fields of this class
        memcpy(buf + bytes, GetUserName(), USERNAME_SIZE);
        bytes += USERNAME_SIZE;

        SERIALIZE_8(tmpChar, IsSaltRequest(), buf, bytes);

        memcpy(buf + bytes, GetPassword(), PASSWORD_SIZE);
        bytes += PASSWORD_SIZE;

        SERIALIZE_32(tmpInt, GetHeartbeatTimeout(), buf, bytes);
        UNSIGNED_SHORT dummyBytes = 0;
        // Put size as the first field after deducting 2 bytes reserved for size
        SERIALIZE_16(tmpShort, (UNSIGNED_SHORT) (bytes - sizeof(UNSIGNED_SHORT)), buf, dummyBytes);

        return bytes;
    }

/////////////////////////////////////////////////////////////////



    ScripMasterDataRequest::ScripMasterDataRequest(const char *buf) {
        int offset = 0;
        UNSIGNED_LONG tmpLong1 = 0, tmpLong2 = 0;
        UNSIGNED_INTEGER tmpInt1 = 0, tmpInt2 = 0;
        float tmpfloat1 = 0.0f, tmpfloat2 = 0.0f;

        SetSymbol(strdup_internal(buf + offset));
        offset += SYMBOL_SIZE;
        DESERIALIZE_FLOAT(tmpfloat1, tmpfloat2, SetTickSize(tmpfloat2), buf, offset);
        DESERIALIZE_64(tmpLong1, tmpLong2, SetLotSize(tmpLong2), buf, offset);
        DESERIALIZE_32(tmpInt1, tmpInt2, SetPrecission(tmpInt2), buf, offset);
    }

    int ScripMasterDataRequest::Serialize(char *buf) {
        int bytes = 0;
        UNSIGNED_SHORT tmpShort = 0;
        UNSIGNED_CHARACTER tmpChar = 0;
        UNSIGNED_LONG tmpLong = 0;
        UNSIGNED_INTEGER tmpInt = 0;
        float tmpfloat = 0.0f;

        bytes = sizeof(UNSIGNED_SHORT);  // Leave 2 bytes for packet size
        // Put category of command
        SERIALIZE_8(tmpChar, (UNSIGNED_CHARACTER) (CommandCategory_SEND_SCRIP_MASTER_DATA), buf,
                    bytes);  //Command Category

        memcpy(buf + bytes, GetSymbol(), SYMBOL_SIZE);
        bytes += SYMBOL_SIZE;
        SERIALIZE_FLOAT(tmpfloat, GetTickSize(), buf, bytes);
        SERIALIZE_64(tmpLong, GetLotSize(), buf, bytes);
        SERIALIZE_32(tmpInt, GetPrecission(), buf, bytes);


        UNSIGNED_SHORT dummyBytes = 0;
        // Put size as the first field after deducting 2 bytes reserved for size
        SERIALIZE_16(tmpShort, (UNSIGNED_SHORT) (bytes - sizeof(UNSIGNED_SHORT)), buf, dummyBytes);

        return bytes;
    }
#ifndef FRONTEND
    void ScripMasterDataRequest::dump() {
        LOG(INFO) << "ScripMasterDataRequest Dump : Symbol [" << GetSymbol()
        << "] Tick size [" << GetTickSize()
        << "] Lot Size [" << GetLotSize()
        << "] Precision [" << GetPrecission()
        << "]";
    }
#endif

    int AlgoModelStrategyId::serialize(char *buf) {
        int bytes = 0;
        UNSIGNED_CHARACTER tmpChar = 0;
        UNSIGNED_LONG tmpLong = 0;
        UNSIGNED_SHORT tmpShort = 0;

        bytes = sizeof(UNSIGNED_SHORT);  // Leave 2 bytes for packet size
        // Put category of command
        SERIALIZE_8(tmpChar, (UNSIGNED_CHARACTER) (CommandCategory_STRATEGY_ID), buf, bytes);  //Command Category

        SERIALIZE_64(tmpLong, getStrategyId(), buf, bytes);
        SERIALIZE_64(tmpLong, getInternalId(), buf, bytes);

        UNSIGNED_SHORT dummyBytes = 0;
        // Put size as the first field after deducting 2 bytes reserved for size
        SERIALIZE_16(tmpShort, (UNSIGNED_SHORT) (bytes - sizeof(UNSIGNED_SHORT)), buf, dummyBytes);

        return bytes;
    }

    AlgoModelStrategyId::AlgoModelStrategyId(const char *buf) {
        int offset = 0;
        UNSIGNED_LONG tmpLong1 = 0, tmpLong2 = 0;
        DESERIALIZE_64(tmpLong1, tmpLong2, setStrategyId(tmpLong2), buf, offset);
        DESERIALIZE_64(tmpLong1, tmpLong2, setInternalId(tmpLong2), buf, offset);
    }

    int AlgoModelStopStrategy::serialize(char *buf) {
        int bytes = 0;
        UNSIGNED_CHARACTER tmpChar = 0;
        UNSIGNED_LONG tmpLong = 0;
        UNSIGNED_SHORT tmpShort = 0;

        bytes = sizeof(UNSIGNED_SHORT);  // Leave 2 bytes for packet size
        // Put category of command
        SERIALIZE_8(tmpChar, (UNSIGNED_CHARACTER) (CommandCategory_STOP_STRATEGY), buf, bytes);  //Command Category

        SERIALIZE_64(tmpLong, getStrategyId(), buf, bytes);

        UNSIGNED_SHORT dummyBytes = 0;
        // Put size as the first field after deducting 2 bytes reserved for size
        SERIALIZE_16(tmpShort, (UNSIGNED_SHORT) (bytes - sizeof(UNSIGNED_SHORT)), buf, dummyBytes);

        return bytes;
    }

    AlgoModelStopStrategy::AlgoModelStopStrategy(const char *buf) {
        int offset = 0;
        UNSIGNED_LONG tmpLong1 = 0, tmpLong2 = 0;
        DESERIALIZE_64(tmpLong1, tmpLong2, setStrategyId(tmpLong2), buf, offset);
    }

    AlgoModel1::AlgoModel1(const char *buf) {
        int offset = 0;
        UNSIGNED_LONG tmpLong1 = 0, tmpLong2 = 0;
        UNSIGNED_INTEGER tmpInt1 = 0, tmpInt2 = 0;
        UNSIGNED_CHARACTER tmpChar;
        float tmpFloat1 = 0.0f, tmpFloat2 = 0.0f;
        double tmpDouble1 = 0.0, tmpDouble2 = 0.0;

        DESERIALIZE_64(tmpLong1, tmpLong2, SetStrategyId(tmpLong2), buf, offset);
        DESERIALIZE_64(tmpLong1, tmpLong2, SetInternalId(tmpLong2), buf, offset);

        SetDataSymbol(strdup_internal(buf + offset));
        offset += SYMBOL_SIZE;
        SetDataSecType(strdup_internal(buf + offset));
        offset += SYMBOL_SIZE;
        SetDataCurrency(strdup_internal(buf + offset));
        offset += SYMBOL_SIZE;
        SetDataExchange(strdup_internal(buf + offset));
        offset += SYMBOL_SIZE;

        DESERIALIZE_FLOAT(tmpFloat1, tmpFloat2, SetTickSize(tmpFloat1), buf, offset);
        DESERIALIZE_8(tmpChar, SetMode(tmpChar), buf, offset);
        DESERIALIZE_DOUBLE(tmpDouble1, tmpDouble2, SetPrevClose(tmpDouble2), buf, offset);
        DESERIALIZE_DOUBLE(tmpDouble1, tmpDouble2, SetBaseValue(tmpDouble2), buf, offset);
        DESERIALIZE_DOUBLE(tmpDouble1, tmpDouble2, SetEntryPrice(tmpDouble2), buf, offset);
        DESERIALIZE_DOUBLE(tmpDouble1, tmpDouble2, SetEntryPriceCalc(tmpDouble2), buf, offset);
        DESERIALIZE_DOUBLE(tmpDouble1, tmpDouble2, SetPointA(tmpDouble2), buf, offset);
        DESERIALIZE_DOUBLE(tmpDouble1, tmpDouble2, SetPointACalc(tmpDouble2), buf, offset);
        DESERIALIZE_DOUBLE(tmpDouble1, tmpDouble2, SetPointB(tmpDouble2), buf, offset);
        DESERIALIZE_DOUBLE(tmpDouble1, tmpDouble2, SetPointBCalc(tmpDouble2), buf, offset);
        DESERIALIZE_DOUBLE(tmpDouble1, tmpDouble2, SetCancelPoint1(tmpDouble2), buf, offset);
        DESERIALIZE_DOUBLE(tmpDouble1, tmpDouble2, SetCancelPoint1Calc(tmpDouble2), buf, offset);
        DESERIALIZE_DOUBLE(tmpDouble1, tmpDouble2, SetCancelPoint2(tmpDouble2), buf, offset);
        DESERIALIZE_DOUBLE(tmpDouble1, tmpDouble2, SetCancelPoint2Calc(tmpDouble2), buf, offset);
        DESERIALIZE_DOUBLE(tmpDouble1, tmpDouble2, setParentOrderPrice(tmpDouble2), buf, offset);
        DESERIALIZE_DOUBLE(tmpDouble1, tmpDouble2, SetProfit(tmpDouble2), buf, offset);
        DESERIALIZE_DOUBLE(tmpDouble1, tmpDouble2, SetProfitCalc(tmpDouble2), buf, offset);
        DESERIALIZE_DOUBLE(tmpDouble1, tmpDouble2, SetLoss(tmpDouble2), buf, offset);
        DESERIALIZE_DOUBLE(tmpDouble1, tmpDouble2, SetLossCalc(tmpDouble2), buf, offset);
        DESERIALIZE_DOUBLE(tmpDouble1, tmpDouble2, SetLossLimitPrice(tmpDouble2), buf, offset);
        DESERIALIZE_DOUBLE(tmpDouble1, tmpDouble2, SetTolerance(tmpDouble2), buf, offset);

        DESERIALIZE_32(tmpInt1, tmpInt2, SetTimeEnabled(tmpInt2), buf, offset);

        SetStartTime(strdup_internal(buf + offset));
        offset += TIME_SIZE;

        SetEndTime(strdup_internal(buf + offset));
        offset += TIME_SIZE;

        SetStartDate(strdup_internal(buf + offset));
        offset += TIME_SIZE;

        SetEndDate(strdup_internal(buf + offset));
        offset += TIME_SIZE;

        DESERIALIZE_64(tmpLong1, tmpLong2, SetStartDateEpoch(tmpLong2), buf, offset);
        DESERIALIZE_64(tmpLong1, tmpLong2, SetEndDateEpoch(tmpLong2), buf, offset);
        DESERIALIZE_64(tmpLong1, tmpLong2, SetStartTimeEpoch(tmpLong2), buf, offset);
        DESERIALIZE_64(tmpLong1, tmpLong2, SetEndTimeEpoch(tmpLong2), buf, offset);

        DESERIALIZE_8(tmpChar, SetStatus(tmpChar), buf, offset);
        DESERIALIZE_64(tmpLong1, tmpLong2, SetQty(tmpLong2), buf, offset);
        DESERIALIZE_64(tmpLong1, tmpLong2, SetMaxQty(tmpLong2), buf, offset);


        SetPointATime(strdup_internal(buf + offset));
        offset += TIME_SIZE;

        SetPointBTime(strdup_internal(buf + offset));
        offset += TIME_SIZE;

        SetCancelTime(strdup_internal(buf + offset));
        offset += TIME_SIZE;

        DESERIALIZE_64(tmpLong1, tmpLong2, SetCancelTicks(tmpLong2), buf, offset);

        SetCPATime(strdup_internal(buf + offset));
        offset += TIME_SIZE;

        SetCPBTime(strdup_internal(buf + offset));
        offset += TIME_SIZE;

        DESERIALIZE_DOUBLE(tmpDouble1, tmpDouble2, SetTrailing(tmpDouble2), buf, offset);
        DESERIALIZE_DOUBLE(tmpDouble1, tmpDouble2, SetTrailingPercentage(tmpDouble2), buf, offset);
        DESERIALIZE_8(tmpChar, SetAlgoType(tmpChar), buf, offset);

        SetUserName(strdup_internal(buf + offset));
        offset += CLIENT_NAME_SIZE;

        SetDailyOrWeekly(strdup_internal(buf + offset));
        offset += DAILY_WEEKLY_SIZE;

        SetIdentifier(strdup_internal(buf + offset));
        offset += IDENTIFIER_SIZE;

        SetUpname(strdup_internal(buf + offset));
        offset += UPLOAD_FILE_NAME_SIZE;

        setSecondaryOrderExitDate(strdup_internal(buf + offset));
        offset += TIME_SIZE;

        setSecondaryOrderExitTime(strdup_internal(buf + offset));
        offset += TIME_SIZE;

        DESERIALIZE_64(tmpLong1, tmpLong2, setSecondaryOrderExitDateEpoch(tmpLong2), buf,
                       offset);
        DESERIALIZE_64(tmpLong1, tmpLong2, setSecondaryOrderExitTimeEpoch(tmpLong2), buf, offset);
        DESERIALIZE_32(tmpInt1, tmpInt2, setSecondaryOrderExitEnabled(tmpInt2), buf, offset);

        SetEntryPriceTime(strdup_internal(buf + offset));
        offset += TIME_SIZE;

        SetStopTime(strdup_internal(buf + offset));
        offset += TIME_SIZE;

        SetLimitTime(strdup_internal(buf + offset));
        offset += TIME_SIZE;
    }


    int AlgoModel1::Serialize(char *buf) {
        int bytes = 0;
        UNSIGNED_SHORT tmpShort = 0;
        UNSIGNED_CHARACTER tmpChar = 0;
        UNSIGNED_INTEGER tmpInt = 0;
        UNSIGNED_LONG tmpLong = 0;
        float tmpFloat = 0.0f;
        double tmpDouble = 0.0;

        bytes = sizeof(UNSIGNED_SHORT);  // Leave 2 bytes for packet size
        // Put category of command
        SERIALIZE_8(tmpChar, (UNSIGNED_CHARACTER) (CommandCategory_ALGO_MODEL_1), buf, bytes);  //Command Category

        SERIALIZE_64(tmpLong, GetStrategyId(), buf, bytes);
        SERIALIZE_64(tmpLong, GetInternalId(), buf, bytes);

        memset(buf + bytes, '\0', SYMBOL_SIZE);
        memcpy(buf + bytes, GetDataSymbol(), SYMBOL_SIZE);
        bytes += SYMBOL_SIZE;
        memset(buf + bytes, '\0', SYMBOL_SIZE);
        memcpy(buf + bytes, GetDataSecType(), SYMBOL_SIZE);
        bytes += SYMBOL_SIZE;
        memset(buf + bytes, '\0', SYMBOL_SIZE);
        memcpy(buf + bytes, GetDataCurrency(), SYMBOL_SIZE);
        bytes += SYMBOL_SIZE;
        memset(buf + bytes, '\0', SYMBOL_SIZE);
        memcpy(buf + bytes, GetDataExchange(), SYMBOL_SIZE);
        bytes += SYMBOL_SIZE;

        SERIALIZE_FLOAT(tmpFloat, GetTickSize(), buf, bytes);
        SERIALIZE_8(tmpChar, GetMode(), buf, bytes);
        SERIALIZE_DOUBLE(tmpDouble, GetPrevClose(), buf, bytes);
        SERIALIZE_DOUBLE(tmpDouble, GetBaseValue(), buf, bytes);
        SERIALIZE_DOUBLE(tmpDouble, GetEntryPrice(), buf, bytes);
        SERIALIZE_DOUBLE(tmpDouble, GetEntryPriceCalc(), buf, bytes);
        SERIALIZE_DOUBLE(tmpDouble, GetPointA(), buf, bytes);
        SERIALIZE_DOUBLE(tmpDouble, GetPointACalc(), buf, bytes);
        SERIALIZE_DOUBLE(tmpDouble, GetPointB(), buf, bytes);
        SERIALIZE_DOUBLE(tmpDouble, GetPointBCalc(), buf, bytes);
        SERIALIZE_DOUBLE(tmpDouble, GetCancelPoint1(), buf, bytes);
        SERIALIZE_DOUBLE(tmpDouble, GetCancelPoint1Calc(), buf, bytes);
        SERIALIZE_DOUBLE(tmpDouble, GetCancelPoint2(), buf, bytes);
        SERIALIZE_DOUBLE(tmpDouble, GetCancelPoint2Calc(), buf, bytes);
        SERIALIZE_DOUBLE(tmpDouble, getParentOrderPrice(), buf, bytes);
        SERIALIZE_DOUBLE(tmpDouble, GetProfit(), buf, bytes);
        SERIALIZE_DOUBLE(tmpDouble, GetProfitCalc(), buf, bytes);
        SERIALIZE_DOUBLE(tmpDouble, GetLoss(), buf, bytes);
        SERIALIZE_DOUBLE(tmpDouble, GetLossCalc(), buf, bytes);
        SERIALIZE_DOUBLE(tmpDouble, GetLossLimitPrice(), buf, bytes);
        SERIALIZE_DOUBLE(tmpDouble, GetTolerance(), buf, bytes);

        SERIALIZE_32(tmpInt, GetIsTimeEnabled(), buf, bytes);
        memset(buf + bytes, '\0', TIME_SIZE);
        memcpy(buf + bytes, GetStartTime(), TIME_SIZE);
        bytes += TIME_SIZE;
        memset(buf + bytes, '\0', TIME_SIZE);
        memcpy(buf + bytes, GetEndTime(), TIME_SIZE);
        bytes += TIME_SIZE;

        memset(buf + bytes, '\0', TIME_SIZE);
        memcpy(buf + bytes, GetStartDate(), TIME_SIZE);
        bytes += TIME_SIZE;
        memset(buf + bytes, '\0', TIME_SIZE);
        memcpy(buf + bytes, GetEndDate(), TIME_SIZE);
        bytes += TIME_SIZE;


        SERIALIZE_64(tmpLong, GetStartDateEpoch(), buf, bytes);
        SERIALIZE_64(tmpLong, GetEndDateEpoch(), buf, bytes);
        SERIALIZE_64(tmpLong, GetStartTimeEpoch(), buf, bytes);
        SERIALIZE_64(tmpLong, GetEndTimeEpoch(), buf, bytes);

        SERIALIZE_8(tmpChar, GetStatus(), buf, bytes);
        SERIALIZE_64(tmpLong, GetQty(), buf, bytes);
        SERIALIZE_64(tmpLong, GetMaxQty(), buf, bytes);

        memset(buf + bytes, '\0', TIME_SIZE);
        memcpy(buf + bytes, GetPointATime(), TIME_SIZE);
        bytes += TIME_SIZE;
        memset(buf + bytes, '\0', TIME_SIZE);
        memcpy(buf + bytes, GetPointBTime(), TIME_SIZE);
        bytes += TIME_SIZE;
        memset(buf + bytes, '\0', TIME_SIZE);
        memcpy(buf + bytes, GetCancelTime(), TIME_SIZE);
        bytes += TIME_SIZE;

        SERIALIZE_64(tmpLong, GetCancelTicks(), buf, bytes);

        memset(buf + bytes, '\0', TIME_SIZE);
        memcpy(buf + bytes, GetCPATime(), TIME_SIZE);
        bytes += TIME_SIZE;

        memset(buf + bytes, '\0', TIME_SIZE);
        memcpy(buf + bytes, GetCPBTime(), TIME_SIZE);
        bytes += TIME_SIZE;


        SERIALIZE_DOUBLE(tmpDouble, GetTrailingStop(), buf, bytes);
        SERIALIZE_DOUBLE(tmpDouble, getTrailingPercentage(), buf, bytes);
        SERIALIZE_8(tmpChar, GetAlgoType(), buf, bytes);

        memset(buf + bytes, '\0', CLIENT_NAME_SIZE);
        memcpy(buf + bytes, GetUserName(), CLIENT_NAME_SIZE);
        bytes += CLIENT_NAME_SIZE;
	memset(buf + bytes, '\0', DAILY_WEEKLY_SIZE);
	memcpy(buf + bytes, GetDailyOrWeekly(), DAILY_WEEKLY_SIZE);
	bytes += DAILY_WEEKLY_SIZE;

        memset(buf + bytes, '\0', IDENTIFIER_SIZE);
        memcpy(buf + bytes, GetIdentifier(), IDENTIFIER_SIZE);
        bytes += IDENTIFIER_SIZE;

        memset(buf + bytes, '\0', UPLOAD_FILE_NAME_SIZE);
        memcpy(buf + bytes, GetUpname(), UPLOAD_FILE_NAME_SIZE);
        bytes += UPLOAD_FILE_NAME_SIZE;

        memset(buf + bytes, '\0', TIME_SIZE);
        memcpy(buf + bytes, getSecondaryOrderExitDate().c_str(), TIME_SIZE);
        bytes += TIME_SIZE;
        memset(buf + bytes, '\0', TIME_SIZE);
        memcpy(buf + bytes, getSecondaryOrderExitTime().c_str(), TIME_SIZE);
        bytes += TIME_SIZE;

        SERIALIZE_64(tmpLong, getSecondaryOrderExitDateEpoch(), buf, bytes);
        SERIALIZE_64(tmpLong, getSecondaryOrderExitTimeEpoch(), buf, bytes);
        SERIALIZE_32(tmpInt, getSecondaryOrderExitEnabled(), buf, bytes);

        memset(buf + bytes, '\0', TIME_SIZE);
        memcpy(buf + bytes, GetEntryPriceTime(), TIME_SIZE);
        bytes += TIME_SIZE;

        memset(buf + bytes, '\0', TIME_SIZE);
        memcpy(buf + bytes, GetStopTime(), TIME_SIZE);
        bytes += TIME_SIZE;

        memset(buf + bytes, '\0', TIME_SIZE);
        memcpy(buf + bytes, GetLimitTime(), TIME_SIZE);
        bytes += TIME_SIZE;

        UNSIGNED_SHORT dummyBytes = 0;
        // Put size as the first field after deducting 2 bytes reserved for size
        SERIALIZE_16(tmpShort, (UNSIGNED_SHORT) (bytes - sizeof(UNSIGNED_SHORT)), buf, dummyBytes);

        return bytes;
    }
#ifndef FRONTEND
    void AlgoModel1::Dump(std::string prefix) {
        LOG(INFO) << prefix << "AlgoModel1 Dump :";
        LOG(INFO) << prefix << "StrategyId        " << GetStrategyId();
        LOG(INFO) << prefix << "InternalId        " << GetInternalId();
        LOG(INFO) << prefix << "DataSymbol [" << GetDataSymbol() << "," << GetDataSecType()
                          << "," << GetDataExchange() << "," << GetDataCurrency() << "]";
        LOG(INFO) << prefix << "Tick Size         " << GetTickSize();
        LOG(INFO) << prefix << "Mode              " << OrderModeStr[(int) GetMode()];
        LOG(INFO) << prefix << "Prev Close        " << GetPrevClose();
        LOG(INFO) << prefix << "Base Val          " << GetBaseValue();
        LOG(INFO) << prefix << "Entry Price Given " << GetEntryPrice();
        LOG(INFO) << prefix << "Entry Price Calc  " << GetEntryPriceCalc();
        LOG(INFO) << prefix << "PointA            " << GetPointA();
        LOG(INFO) << prefix << "PointACalc        " << GetPointACalc();
        LOG(INFO) << prefix << "PointB            " << GetPointB();
        LOG(INFO) << prefix << "PointB Calc       " << GetPointBCalc();
        LOG(INFO) << prefix << "CP1               " << GetCancelPoint1();
        LOG(INFO) << prefix << "CP1 Calc          " << GetCancelPoint1Calc();
        LOG(INFO) << prefix << "CP2               " << GetCancelPoint2();
        LOG(INFO) << prefix << "CP2 Calc          " << GetCancelPoint2Calc();
        LOG(INFO) << prefix << "ParentOrder Price " << getParentOrderPrice();
        LOG(INFO) << prefix << "Profit            " << GetProfit();
        LOG(INFO) << prefix << "Profit Calc       " << GetProfitCalc();
        LOG(INFO) << prefix << "Loss              " << GetLoss();
        LOG(INFO) << prefix << "Loss Calc         " << GetLossCalc();
        LOG(INFO) << prefix << "Loss limit price  " << GetLossLimitPrice();
        LOG(INFO) << prefix << "Tolerance         " << GetTolerance();
        LOG(INFO) << prefix << "Start Time        " << std::string(GetStartTime());
        LOG(INFO) << prefix << "End Time          " << std::string(GetEndTime());

        LOG(INFO) << prefix << "Start Time Epoch  " << GetStartTimeEpoch();
        LOG(INFO) << prefix << "End Time Epoch    " << GetEndTimeEpoch();

        LOG(INFO) << prefix << "Status            " << (int) GetStatus();
        LOG(INFO) << prefix << "Qty               " << GetQty();
        LOG(INFO) << prefix << "Max Qty           " << GetMaxQty();
        LOG(INFO) << prefix << "Cancel Ticks      " << GetCancelTicks();
        LOG(INFO) << prefix << "Cancel Point A At " << GetCPATime();
        LOG(INFO) << prefix << "Cancel Point B At " << GetCPBTime();
        LOG(INFO) << prefix << "Trailing Stop     " << GetTrailingStop();
        LOG(INFO) << prefix << "Trailing Stop Percentage " << getTrailingPercentage();
        LOG(INFO) << prefix << "Algo Type         " << AlgoTypeStr[(int) GetAlgoType()];
        LOG(INFO) << prefix << "Start date        " << GetStartDate();
        LOG(INFO) << prefix << "End date          " << GetEndDate();
        LOG(INFO) << prefix << "UserName          " << GetUserName();
        LOG(INFO) << prefix << "Daily/Weekly      " << GetDailyOrWeekly();
        LOG(INFO) << prefix << "Identifier        " << GetIdentifier();
        LOG(INFO) << prefix << "Upload file Name  " << GetUpname();
        LOG(INFO) << prefix << "SecondaryExitDate " << getSecondaryOrderExitDate();
        LOG(INFO) << prefix << "SecondaryExitTime " << getSecondaryOrderExitTime();
        LOG(INFO) << prefix << "SecondaryExitEnabled " << getSecondaryOrderExitEnabled();
    }
#endif
#ifdef FRONTEND
    void AlgoModel1::Dump()
    {
        qDebug() << "AlgoModel1 dump :";
        qDebug() << " StrategyId " << GetStrategyId();
        qDebug() << " InternalId " << GetInternalId();
        qDebug() << " DataSymbol [" << GetDataSymbol() << "," << GetDataSecType()
                         << "," << GetDataExchange() << "," << GetDataCurrency() << "]";
        qDebug() << " Tick Size " << GetTickSize();
        qDebug() << " Mode " << QString(OrderModeStr[(int) GetMode()].c_str());
        qDebug() << " Prev Close " << GetPrevClose();
        qDebug() << " Base Val " << GetBaseValue();
        qDebug() << " Entry Price Given "<< GetEntryPrice();
        qDebug() << " Entry Price Calc "<< GetEntryPriceCalc();
        qDebug() << " PointA " << GetPointA();
        qDebug() << " PointACalc " << GetPointACalc();
        qDebug() << " PointB " << GetPointB();
        qDebug() << " PointB Calc " << GetPointBCalc();
        qDebug() << " CP1 " <<  GetCancelPoint1();
        qDebug() << " CP1 Calc "<< GetCancelPoint1Calc();
        qDebug() << " CP2 " << GetCancelPoint2();
        qDebug() << " CP2 Calc " << GetCancelPoint2Calc();
        qDebug() << " ParentOrder Price " << getParentOrderPrice();
        qDebug() << " Profit " << GetProfit();
        qDebug() << " Profit Calc " << GetProfitCalc();
        qDebug() << " Loss " << GetLoss();
        qDebug() << " Loss Calc " << GetLossCalc();
        qDebug() << " Loss limit price " << GetLossLimitPrice();
        qDebug() << " Tolerance " << GetTolerance();
        qDebug() << " UserName " << GetUserName();
        qDebug() << " Status " << (int)GetStatus();

        qDebug() << " Start Date        " << GetStartDate();
        qDebug() << " Start Time        " << GetStartTime();
        qDebug() << " End Date          " << GetEndDate();
        qDebug() << " End Time          " << GetEndTime();

        qDebug() << " Qty " << GetQty();
        qDebug() << " Max Qty " << GetMaxQty();
        qDebug() << " Cancel Ticks      " << GetCancelTicks();
        qDebug() << " Point A Time " << GetPointATime();
        qDebug() << " Point B Time " << GetPointBTime();
        qDebug() << " Cancel Point Time " << GetCancelTime();
        qDebug() << " Cancel Point A At " << GetCPATime();
        qDebug() << " Cancel Point B At " << GetCPBTime();
        qDebug() << " Trailing Stop     " << GetTrailingStop();
        qDebug() << " Trailing Stop Percentage " << getTrailingPercentage();
        qDebug() << " Algo Type         " << QString(AlgoTypeStr[(int) GetAlgoType()].c_str());
        qDebug() << " Daily/Weekly      " << QString(GetDailyOrWeekly());
        qDebug() << " Identifier        " << QString(GetIdentifier());
        qDebug() << " File Name         " << QString(GetUpname());
        qDebug() << " SecondaryExitDate " << QString(getSecondaryOrderExitDate().c_str());
        qDebug() << " SecondaryExitTime " << QString(getSecondaryOrderExitTime().c_str());
        qDebug() << " SecondaryExitEnabled " << getSecondaryOrderExitEnabled();
        qDebug()<< "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";

    }
    void AlgoModel1::DumpToCsv(std::string filename, double pnl, double avgBuyPrice, double avgSellPrice)
    {
        std::ofstream myfile;
        myfile.open(filename.data(), std::ios_base::app);
        if (myfile.is_open())
        {
            myfile << GetUpname() <<"," << GetStrategyId() << "," << GetInternalId() << ","
                   << GetIdentifier() << ","
                   << AlgoTypeStr[(int)GetAlgoType()]
                   << "," << GetDataSymbol() << ","  <<  OrderModeStr[(int) GetMode()] << ","
                   << pnl<<"," << avgSellPrice << "," << avgBuyPrice<< "," << GetPrevClose()
                   << "," << GetBaseValue() << "," << GetEntryPriceCalc()
                   << "," << GetPointACalc() << "," << GetPointBCalc() << ","
                   << GetCancelPoint1Calc() << "," << GetCancelPoint2Calc() << ","
                   << GetProfitCalc() << "," << GetLossCalc() << "," << GetTolerance() <<","
                   << GetStartDate() << "," << GetStartTime() << "," << GetEndDate()
                   << "," << GetEndTime() << "," << GetQty() << "," << GetCancelTicks() << GetTrailingStop()
                   <<"\n";
            myfile.close();
        }
    }
#endif

    Heartbeat::Heartbeat(UNSIGNED_CHARACTER feed, UNSIGNED_CHARACTER op) {
        _isFeedConnected = feed;
        _isOPConnected = op;
    }

    Heartbeat::Heartbeat(const char *buf) //deserializing
    {
        int offset = 0;
        UNSIGNED_CHARACTER tmpChar = 0;

        DESERIALIZE_8(tmpChar, SetFeedConnection(tmpChar), buf, offset);
        DESERIALIZE_8(tmpChar, SetOPConnection(tmpChar), buf, offset);
    }

    int Heartbeat::Serialize(char *buf) {
        int bytes = 0;
        UNSIGNED_CHARACTER tmpChar = 0;
        UNSIGNED_SHORT tmpShort = 0;
        bytes = sizeof(UNSIGNED_SHORT);  // Leave 2 bytes for packet size
        // Put category of command
        SERIALIZE_8(tmpChar, (UNSIGNED_CHARACTER) (CommandCategory_CONNECTION), buf, bytes);  //Command Category

        SERIALIZE_8(tmpChar, GetFeedConnection(), buf, bytes);
        SERIALIZE_8(tmpChar, GetOPConnection(), buf, bytes);

        UNSIGNED_SHORT dummyBytes = 0;
        // Put size as the first field after deducting 2 bytes reserved for size
        SERIALIZE_16(tmpShort, (UNSIGNED_SHORT) (bytes - sizeof(UNSIGNED_SHORT)), buf, dummyBytes);

        return bytes;
    }
#ifndef FRONTEND
    void Heartbeat::Dump() {
        LOG(INFO) << "Heartbeat:"
        << " Feed connected: " << (int) _isFeedConnected
        << " OP connected: " << (int) _isOPConnected;

    }
#endif

    Alerts::Alerts(const char *buf) {
        int offset = 0;
        UNSIGNED_LONG tmpLong1 = 0, tmpLong2 = 0;
        UNSIGNED_CHARACTER tmpChar = 0;
        
        DESERIALIZE_64(tmpLong1, tmpLong2, SetStrategyId(tmpLong2), buf, offset);
        DESERIALIZE_64(tmpLong1, tmpLong2, SetInternalId(tmpLong2), buf, offset);
        DESERIALIZE_64(tmpLong1, tmpLong2, SetTimeStamp(tmpLong2), buf, offset);;

        SetReason(strdup_internal(buf + offset));
        offset += ALERT_MSG_SIZE;
        DESERIALIZE_8(tmpChar, SetAlertType(tmpChar), buf, offset);
        if(GetAlertType() == (UNSIGNED_CHARACTER)AlertType_EXEC_REPORT)
        {
            SetOrderStr(strdup_internal(buf + offset));
            offset += 2048;
        }    
    }


    int Alerts::Serialize(char *buf) {
        int bytes = 0;
        UNSIGNED_LONG tmpLong = 0;
        UNSIGNED_SHORT tmpShort = 0;
        UNSIGNED_CHARACTER tmpChar;

        bytes = sizeof(UNSIGNED_SHORT);  // Leave 2 bytes for packet size
        // Put category of command
        SERIALIZE_8(tmpChar, (UNSIGNED_CHARACTER) (CommandCategory_ALERT), buf, bytes);  //Command Category

        SERIALIZE_64(tmpLong, GetStrategyId(), buf, bytes);
        SERIALIZE_64(tmpLong, GetInternalId(), buf, bytes);
        SERIALIZE_64(tmpLong, GetTimeStamp(), buf, bytes);

        memset(buf + bytes, '\0', ALERT_MSG_SIZE);
        memcpy(buf + bytes, GetReason(), ALERT_MSG_SIZE);
        bytes += ALERT_MSG_SIZE;

        SERIALIZE_8(tmpChar, GetAlertType(), buf, bytes);  //Command Category
        if(GetAlertType() == AlertType_EXEC_REPORT)
        {
            memset(buf + bytes, '\0', 2048);
            memcpy(buf + bytes, GetOrderStr(), 2048);
            bytes += 2048;
        }

        UNSIGNED_SHORT dummyBytes = 0;
        // Put size as the first field after deducting 2 bytes reserved for size
        SERIALIZE_16(tmpShort, (UNSIGNED_SHORT) (bytes - sizeof(UNSIGNED_SHORT)), buf, dummyBytes);

        return bytes;
    }
#ifndef FRONTEND
    void Alerts::Dump(std::string prefix) {
        LOG(INFO) << prefix << "Alert Dump :";
        LOG(INFO) << prefix << "Click ID          " << GetStrategyId();
        LOG(INFO) << prefix << "Sequence ID       " << GetInternalId();
		    LOG(INFO) << prefix << "TimeStamp         " << GetTimeStamp();
        LOG(INFO) << prefix << "Reason            " << std::string(GetReason());
    }
#else
    void Alerts::Dump(std::string filename)
    {
        std::ofstream myfile;
        myfile.open(filename.data(), std::ios_base::app);
        if (!myfile.is_open())
        {
            return;
        }
        myfile << GetStrategyId() << "," << GetInternalId() << ","
               << QDateTime::fromMSecsSinceEpoch((qint64)GetTimeStamp() / 1000, QTimeZone(QByteArray("America/New_York"))).toString("dd.MM.yyyy hh:mm:ss.zzz").toStdString()
               << "," << GetReason()<<"\n";
        myfile.close();
    }
#endif

	OrderResync::OrderResync(const char *buf, bool req) {
		int offset = 0;
		UNSIGNED_LONG tmpLong1 = 0, tmpLong2 = 0;
		if (req)
		{
			DESERIALIZE_64(tmpLong1, tmpLong2, SetLastIndex(tmpLong2), buf, offset);
		}
		else
		{
			SetOrderStr(strdup_internal(buf + offset));
			offset += MAX_BUF;
		}
    }


    int OrderResync::Serialize(char *buf, bool req) {
        int bytes = 0;
        UNSIGNED_SHORT tmpShort = 0;
		    UNSIGNED_LONG tmpLong = 0;
        UNSIGNED_CHARACTER tmpChar;

        bytes = sizeof(UNSIGNED_SHORT);  // Leave 2 bytes for packet size
        // Put category of command
		if (req)
		{
			SERIALIZE_8(tmpChar, (UNSIGNED_CHARACTER)(CommandCategory_ORDERRESYNC_REQ), buf, bytes);  //Command Category
			SERIALIZE_64(tmpLong, GetLastIndex(), buf, bytes);
		}
		else
		{
			SERIALIZE_8(tmpChar, (UNSIGNED_CHARACTER)(CommandCategory_ORDERRESYNC_RESP), buf, bytes);  //Command Category
			memset(buf + bytes, '\0', MAX_BUF);
			memcpy(buf + bytes, GetOrderStr(), MAX_BUF);
			bytes += MAX_BUF;
		}

		UNSIGNED_SHORT dummyBytes = 0;
        // Put size as the first field after deducting 2 bytes reserved for size
        SERIALIZE_16(tmpShort, (UNSIGNED_SHORT) (bytes - sizeof(UNSIGNED_SHORT)), buf, dummyBytes);

        return bytes;
    }

	AlgoResync::AlgoResync(const char *buf) {
		int offset = 0;
		UNSIGNED_LONG tmpLong1 = 0, tmpLong2 = 0;
		DESERIALIZE_64(tmpLong1, tmpLong2, SetLastIndex(tmpLong2), buf, offset);
	}


	int AlgoResync::Serialize(char *buf) {
		int bytes = 0;
		UNSIGNED_SHORT tmpShort = 0;
		UNSIGNED_LONG tmpLong = 0;
		UNSIGNED_CHARACTER tmpChar;

		bytes = sizeof(UNSIGNED_SHORT);  // Leave 2 bytes for packet size
		// Put category of command
		SERIALIZE_8(tmpChar, (UNSIGNED_CHARACTER)(CommandCategory_RESYNC_ALGO), buf, bytes);  //Command Category
		SERIALIZE_64(tmpLong, GetLastIndex(), buf, bytes);
		
		UNSIGNED_SHORT dummyBytes = 0;
		// Put size as the first field after deducting 2 bytes reserved for size
		SERIALIZE_16(tmpShort, (UNSIGNED_SHORT)(bytes - sizeof(UNSIGNED_SHORT)), buf, dummyBytes);

		return bytes;
	}


	AlertResync::AlertResync(const char *buf) {
		int offset = 0;
		UNSIGNED_LONG tmpLong1 = 0, tmpLong2 = 0;
		DESERIALIZE_64(tmpLong1, tmpLong2, SetLastIndex(tmpLong2), buf, offset);
	}


	int AlertResync::Serialize(char *buf) {
		int bytes = 0;
		UNSIGNED_SHORT tmpShort = 0;
		UNSIGNED_LONG tmpLong = 0;
		UNSIGNED_CHARACTER tmpChar;

		bytes = sizeof(UNSIGNED_SHORT);  // Leave 2 bytes for packet size
		// Put category of command
        SERIALIZE_8(tmpChar, (UNSIGNED_CHARACTER)(CommandCategory_ALERT), buf, bytes);  //Command Category
		SERIALIZE_64(tmpLong, GetLastIndex(), buf, bytes);

		UNSIGNED_SHORT dummyBytes = 0;
		// Put size as the first field after deducting 2 bytes reserved for size
		SERIALIZE_16(tmpShort, (UNSIGNED_SHORT)(bytes - sizeof(UNSIGNED_SHORT)), buf, dummyBytes);

		return bytes;
	}

    int ConnStatus::Serialize(char *buf)
    {
        int bytes = 0;
        UNSIGNED_CHARACTER tmpChar = 0;
        UNSIGNED_SHORT     tmpShort = 0;
        bytes = sizeof(UNSIGNED_SHORT);  // Leave 2 bytes for packet size
        // Put category of command
        SERIALIZE_8(tmpChar, (UNSIGNED_CHARACTER)(CommandCategory_CONNECTION), buf, bytes);  //Command Category

        UNSIGNED_SHORT dummyBytes = 0;
        // Put size as the first field after deducting 2 bytes reserved for size
        SERIALIZE_16(tmpShort, (UNSIGNED_SHORT)(bytes-sizeof(UNSIGNED_SHORT)), buf, dummyBytes);

        return bytes;
    }
}
