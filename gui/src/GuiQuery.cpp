#include "GuiQuery.h"
#include <QDebug>
#include <algorithm>
#include <sstream>

#include "usersettings.h"

namespace Algo
{

/*!
 * \brief On Startuup, we need to get all Running, Waiting and Open strategies for a given backend instance from db.
 * \return The SQL query to get all these strategies
 */
QString getStartupQuery()
{
    QString guiStartupQuery = "select s.*,i.Name as Instrument from Strategies s,Instruments i "
                              "where s.StrategyStatus in (select Id from StrategyStatuses "
                              "where Name in ('RUNNING','WAITING','OPEN')) and s.InstrumentId=i.Id "
                              "and s.BackendId=" + QString::number(Algo::Settings::getBackendId());
    return guiStartupQuery;
}

/*!
 * \brief We need to get timestamps for PointA, PointB, PointC, CP1, CP2, Limit and Stop fields, i.e. third row events
 * in gui from db only for running strategies. These events may occur at any point of time. So Gui periodically queries db if
 * these events have happened or not.
 * \param lastGuiUpdateTime The last time when Gui got update from db
 * \param strategyList All the running strategies in gui
 * \return The SQL query to get the above timestamps for the running strategies
 */
QString getStrategyThirdRowUpdateQuery(const QString &lastGuiUpdateTime)
{
    QString timeStampCaluse = getTimeStampClause(lastGuiUpdateTime, "i");

    QString strategyWindowUpdateQuery = "select i.StrategyId,i.BackendId,i.EventType,i.Timestamp,NULL \"OrderType\" "
                                        "from InternalEvents i where i.BackendId="
            + QString::number(Algo::Settings::getBackendId()) +
            " and i.EventType in(1,2,3,10,11) "
            + timeStampCaluse +
            "union select o.StrategyId, "
            "o.BackendId, i.EventType,i.Timestamp,o.OrderType from IncomingMessageEvents i, Orders o "
            "where i.ClientOrderId=o.ClientOrderId "
            "and i.BackendId=o.BackendId "
            "and o.BackendId="
            +QString::number(Algo::Settings::getBackendId())+
            " and o.OrderType in ('Primary','Secondary_Limit','Secondary_Stop')" +
            " and i.EventType in (5) "
            + timeStampCaluse +
            "order by Timestamp";
    //qDebug() << "Third window query " << strategyWindowUpdateQuery;
    return strategyWindowUpdateQuery;
}

/*!
 * \brief Gui needs to know StrategyStatus for all strategies since backends update strategy from pending to waiting
 * or pending to running or open. Gui also needs to update accordingly to show it to the user.
 * \param strategyList All the running strategies in gui
 * \return The SQL query to get the status for each strategy
 */
QString getStrategyStatusQuery(const std::vector<int> &strategyList)
{
    QString strategyString = getStrategyString(strategyList);
    QString strategyStatusQuery = "select StrategyId, StrategyStatus from Strategies where StrategyId in("
                                  "" + strategyString + ") and BackendId=" + QString::number(Algo::Settings::getBackendId());
    //qDebug() << "Status " << strategyStatusQuery;
    return strategyStatusQuery;
}

/*!
 * \brief GUI needs to retrieve periodically all alert messages from db. Alert messages have to be retrieved from four tables, i.e.
 * InternalEvents, Orders, IncomingMessageEvents, RejectEvents table. All these messages are to be in order of timestamp. Also an
 * alert to be forme by contacting several columns' values. An example of an alert message is:
 * "Filled Primary Order_EUD/USD_BUY_AlgoType_2_RecvQuantity 1000_RecvPrice 1.234_InternalId_12_StrategyId_15"
 * These events may occur at any point of time. So Gui periodically queries db if these events have happened or not.
 * \param lastGuiUpdateTime The last time when Gui got update from db
 * \return The SQL query to get alerts messages.
 */
QString getAlertWindowUpdateQuery(const QString &lastGuiUpdateTime, const QString &lastId)
{
    QString orderEvents = "select o.Id, s.StrategyStatus, s.StrategyId, s.UploadId, s.UploadName, o.TimeStamp, "
                          "concat_ws('_', concat(\"Sent \",e.Name), "
                          "(case "
                          "when e.Id in (21) "
                          "then "
                          "replace(o.OrderType,'_',' ') "
                          "End), "
                          "convert(ins.Name using utf8), concat(\"Algo \",s.AlgoType), o.Side,"
                          "concat(decimal_trim(o.SentPrice),(case when e.Id in (21) and o.OrderType in ('Secondary_Stop') "
                          "then concat('/',decimal_trim(s.LossLimitPrice)) else \"\" End)), "
                          "decimal_trim(o.SentQuantity), concat(\"InternalId \",s.InternalId), "
                          "concat(\"StrategyId \",s.StrategyId)) As Message from Orders o, Strategies s, "
                          "EventTypes e, Instruments ins where o.BackendId=s.BackendId and o.StrategyId=s.StrategyId and "
                          "o.EventType=e.Id and ins.Id=s.InstrumentId and s.BackendId="
                          + QString::number(Algo::Settings::getBackendId())
                          + getTimeStampClause(lastGuiUpdateTime, "o", lastId);

    QString internalEvents = " select i.Id, s.StrategyStatus, s.StrategyId, s.UploadId, s.UploadName, i.TimeStamp, "
                             "concat_ws('_', convert(e.Name using utf8), "
                             "(case "
                             "when e.Id in (1,2,3,19,20,26,32) "
                             "then "
                             "concat(\"@ \", decimal_trim(i.EventPrice)) "
                             "End), "
                             "ins.Name, concat(\"Algo \",s.AlgoType), s.Side, concat(\"InternalId \",s.InternalId), "
                             "concat(\"StrategyId \",s.StrategyId)) "
                             "As Message from Strategies s, InternalEvents i, EventTypes e, Instruments ins where "
                             "i.BackendId=s.BackendId and i.StrategyId=s.StrategyId and i.EventType=e.Id and "
                             "ins.Id=s.InstrumentId and s.BackendId="
                             + QString::number(Algo::Settings::getBackendId())
                             + getTimeStampClause(lastGuiUpdateTime, "i", lastId);

    QString rejectEvents = " select r.Id, s.StrategyStatus, s.StrategyId, s.UploadId, s.UploadName, r.TimeStamp, "
                           "concat_ws('_', "
                           "concat_ws(' ', "
                           "(case "
                           "when o.EventType in (12) "
                           "then "
                           "concat(\"SECONDARY TEXIT LIMIT\") "
                           "else "
                           "concat(UPPER(replace(o.OrderType,'_',' '))) "
                           "End), "
                           "UPPER(r.Reason)), convert(i.Name using utf8), concat(\"Algo \",s.AlgoType), o.Side, "
                           "decimal_trim(o.SentPrice), decimal_trim(o.SentQuantity), "
                           "concat(\"InternalId \",s.InternalId), concat(\"StrategyId \",s.StrategyId)) "
                           "As Message from RejectEvents r, Strategies s, Orders o, Instruments i where "
                           "r.ClientOrderId=o.ClientOrderId and r.BackendId=o.BackendId and "
                           "s.StrategyId=o.StrategyId and s.BackendId=o.BackendId "
                           "and i.Id=s.InstrumentId and s.BackendId="
                           + QString::number(Algo::Settings::getBackendId())
                           + getTimeStampClause(lastGuiUpdateTime, "r", lastId);

    QString incomingEvents = "select inm.Id, s.StrategyStatus, s.StrategyId, s.UploadId, s.UploadName, inm.TimeStamp, "
                             "concat_ws('_',concat_ws(' ', e.Name, replace(o.OrderType,'_',' ')),convert(ins.Name using utf8), "
                             "concat(\"Algo \",s.AlgoType), o.Side, "
                             "(case "
                             "when e.Id in (5) "
                             "then "
                             "concat_ws('/',decimal_trim(o.SentPrice), decimal_trim(inm.ReceivedPrice)) "
                             "else "
                             "decimal_trim(o.SentPrice) "
                             "End), "
                             "(case "
                             "when e.Id in (5) "
                             "then "
                             "concat_ws('/',decimal_trim(o.SentQuantity), decimal_trim(inm.ReceivedQuantity)) "
                             "else "
                             "decimal_trim(o.SentQuantity) "
                             "End),concat(\"InternalId \",s.InternalId), concat(\"StrategyId \",s.StrategyId)) As Message from "
                             "IncomingMessageEvents inm, Strategies s, EventTypes e, Instruments ins, Orders o  where "
                             "inm.ClientOrderId=o.ClientOrderId and inm.BackendId=o.BackendId and "
                             "o.BackendId=s.BackendId and o.StrategyId=s.StrategyId "
                             "and inm.EventType=e.Id and s.InstrumentId=ins.Id and s.BackendId="
                             + QString::number(Algo::Settings::getBackendId())
                             + getTimeStampClause(lastGuiUpdateTime, "inm", lastId);

    QString alertWindowUpdateQuery = orderEvents
                                     + " union "
                                     + internalEvents
                                     + " union "
                                     + rejectEvents
                                     + " union "
                                     + incomingEvents
                                     + " Order by TimeStamp, Id";
    //qDebug() << "GUI query " << alertWindowUpdateQuery;
    return alertWindowUpdateQuery;
}

/*!
 * \brief This is to get all filled orders from a given day. This query is used for reporting purpose only.
 * \param date This is date from when reports will be generated. This date has to be some-date which should be equal or lower
 * to some start date of a strategy. It will query db from that date and generate reports from that date. For example, lets
 * say if we have loaded strategies all of whose start-date is 2nd Feb, 2017. So date here maybe either 1st or 2nd Feb,
 * 2017 as date.
 * \return The SQL query to get all Filled orders from a given day.
 */
QString getAllFilledOrdersFromAGivenDayQuery(const QString &date)
{
    QString filledOrdersQuery = "select s.*,ins.Name as Instrument, o.ClientOrderId, o.OrderType, "
                                "o.SentQuantity, o.sentPrice, inm.ReceivedQuantity, inm.ReceivedPrice,inm.TimeStamp, "
                                "b.BuyAccount, b.SellAccount from Strategies s,Instruments ins, Orders o, "
                                "IncomingMessageEvents inm, Backends b where s.InstrumentId=ins.Id and s.BackendId="
                                + QString::number(Algo::Settings::getBackendId())
                                + " and s.BackendId=o.BackendId and s.StrategyId=o.StrategyId and o.ClientOrderId="
                                  "inm.ClientOrderId and o.BackendId=inm.BackendId and b.Id="
                                + QString::number(Algo::Settings::getBackendId())
                                +" and inm.EventType in (5) and s.StartDate >='"
                                + date + "' Order by inm.TimeStamp";
    return filledOrdersQuery;
}

/*!
 * \brief This is to get all open position strategies from a given day. This query is used for reporting purpose only.
 * \param date This is date from when reports will be generated. This date has to be some-date which should be equal or lower
 * to some start date of a strategy. It will query db from that date and generate reports from that date. For example, lets
 * say if we have loaded strategies all of whose start-date is 2nd Feb, 2017. So date here maybe either 1st or 2nd Feb,
 * 2017 as date.
 * \return The SQL query to get all open positions from a given day.
 */
QString getOpenPositionsFromAGivenDayQuery(const QString &date)
{
    QString openPositionQuery = "select s.*,ins.Name as Instrument, o.SentQuantity, o.sentPrice, inm.ReceivedQuantity, "
                                "inm.TimeStamp, inm.ReceivedPrice from Strategies s,Instruments ins, Orders o, "
                                "IncomingMessageEvents inm where s.StrategyStatus in (select Id from StrategyStatuses where Name "
                                "in ('OPEN')) and s.InstrumentId=ins.Id and s.BackendId="
                                + QString::number(Algo::Settings::getBackendId())
                                + " and s.BackendId=o.BackendId and s.StrategyId=o.StrategyId and o.EventType=21 "
                                "and o.OrderType='Primary' and o.ClientOrderId=inm.ClientOrderId and o.BackendId=inm.BackendId and "
                                "inm.EventType=5"
                                + " and s.StartDate >='" + date + "' Order by inm.TimeStamp";
    qDebug() << "Open " << openPositionQuery;
    return openPositionQuery;
}

/*!
 * \brief This is to get all stopped strategies from a given day
 * \param date This is date from when reports will be generated. This date has to be some-date which should be equal or lower
 * to some start date of a strategy. It will query db from that date and generate reports from that date. For example, lets
 * say if we have loaded strategies all of whose start-date is 2nd Feb, 2017. So date here maybe either 1st or 2nd Feb,
 * 2017 as date.
 * \return The SQL query to get all closed strategies from a given day.
 */
QString getAllClosedStrategiesFromAGivenDayQuery(const QString &date)
{
    QString closedStrategiesQuery = "select s.*,i.Name as Instrument from Strategies s,Instruments i "
                                    "where s.StrategyStatus in (select Id from StrategyStatuses "
                                    "where Name in ('STOPPED')) and s.InstrumentId=i.Id"
                                    " and s.BackendId=" + QString::number(Algo::Settings::getBackendId())
                                    + " and s.StartDate >='" + date + "'";
    return closedStrategiesQuery;
}

/*!
 * \brief This is to get All Filled Orders i.e. Partial or Full for both primary and secondary orders for a given strategy
 * \param strategyId This is the strategyId for which all filled orders will be fetched from db.
 * \return The SQL query to get all filled order report for a given strategy.
 */
QString getAllFilledOrdersForAGivenStrategy(const QString &strategyId)
{
    QString getAllFilledOrdersQuery = "select o.OrderType, o.Side, o.SentQuantity, o.sentPrice, i.ReceivedQuantity, "
                                      "i.ReceivedPrice from Orders o, IncomingMessageEvents i where "
                                      "o.ClientOrderId=i.ClientOrderId and o.StrategyId="
                                      + strategyId + " and o.BackendId="
                                      + QString::number(Algo::Settings::getBackendId()) +
                                      " and i.EventType in (5)";
    return getAllFilledOrdersQuery;
}

QString getTimeStampClause(const QString &lastGuiUpdateTime, const QString &prefix)
{
    QString timeStampCaluse = lastGuiUpdateTime.isEmpty()?
                "" :
                " and substr(date_format(" + prefix +".TimeStamp, '%Y-%m-%d %H:%i:%s.%f'),1,23) > '" + lastGuiUpdateTime + "' ";
    return timeStampCaluse;
}

QString getTimeStampClause(const QString &lastGuiUpdateTime, const QString &prefix, const QString &lastId)
{
    QString caluse = "";
    if (!lastId.isEmpty())
    {
        caluse = " and " + prefix +".Id > " + lastId;
    }
    else if (!lastGuiUpdateTime.isEmpty())
    {
        caluse = " and substr(date_format(" + prefix +".TimeStamp, '%Y-%m-%d %H:%i:%s.%f'),1,23) >= '" + lastGuiUpdateTime + "' ";
    }
    return caluse;
}

QString getStrategyString(const std::vector<int> &strategyList)
{
    std::stringstream list;
    std::copy(strategyList.begin(), strategyList.end(), std::ostream_iterator<int>(list, ","));
    QString strategyString = list.str().substr(0, list.str().length() - 1).c_str();

    return strategyString;
}
}
