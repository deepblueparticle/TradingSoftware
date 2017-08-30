#include "ReportGenerator.h"
#include "usersettings.h"
#include "DatabaseConnection.h"
#include "GuiQuery.h"
#include "shared/commands.h"
#include <QDir>
#include <QDebug>
#include <stdlib.h>
#include <fstream>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlRecord>
#include <QDate>
#include <QDateTime>
#include <QTime>
#include <map>

namespace Algo
{
ReportGenerator::ReportGenerator(const QString &date) :
    _startDate(date)
{

}

void ReportGenerator::run()
{
    QString applicationPath = QDir::currentPath();
    QString reportFilesPath = applicationPath + "/" + "Reporting/";
    if (!QDir(reportFilesPath).exists()) {
        QDir().mkdir(reportFilesPath);
    }
    QString currentTime = QDateTime::currentDateTime().toString("_yyyyMMdd_hhmmss");
    createClosedStrategyReport(reportFilesPath + "Closed_Strategies" + currentTime + ".csv");
    createOpenPositionReport(reportFilesPath + "Open_positions" + currentTime + ".csv");
    createFilledOrderReport(reportFilesPath + "Filled_Order" + currentTime + ".csv");
    createEventReport(reportFilesPath + "Events" + currentTime + ".csv");

    emit resultReady("Reports have been generated!");
}

void ReportGenerator::createEventReport(const QString &applicationPath)
{
    DatabaseConnection &dbConnection = DatabaseConnection::getInstance();
    std::ofstream myfile;
    std::map<QString, double> quantityMap;

    myfile.open(applicationPath.toStdString());

    if (myfile.is_open())
    {
        myfile << "UploadID, UploadName, TimeStamp, Events\n";
        QSqlQuery query = dbConnection.executeQuery(getAlertWindowUpdateQuery(_startDate + " 00:00:00.000", ""));

        while (query.next()) {
            myfile << query.value(3).toString().toStdString() << ","
                   << query.value(4).toString().toStdString() << ","
                   << query.value(5).typeName() << ","
                   << query.value(6).toString().toStdString() << "\n";
        }
    }
    myfile.close();
    myfile.clear();
}

void ReportGenerator::createFilledOrderReport(const QString &applicationPath)
{
    DatabaseConnection &dbConnection = DatabaseConnection::getInstance();
    std::ofstream myfile;
    std::map<QString, double> quantityMap;
    std::map<qlonglong, double> priceMap;

    myfile.open(applicationPath.toStdString());

    if (myfile.is_open())
    {
        myfile << "UploadName, TimeStamp, Instrument, PipCount, AlgoType, Side, Type, Point C Price, Order Price, "
                  "Fill Price, Slippage, OrderQuantity, Filled Quantity, Status, ClientOrderId, UploadId, InternalId, "
                  "StrategyId, Account\n";
    }
    QSqlQuery query = dbConnection.executeQuery(getAllFilledOrdersFromAGivenDayQuery(_startDate));

    while (query.next()) {
        QSqlRecord name = query.record();
        int side  = (query.value(name.indexOf("Side")).toString() == "BUY"?Algo::OrderMode_BUY:Algo::OrderMode_SELL);
        double sentPrice = query.value(name.indexOf("sentPrice")).toDouble();
        double recvPrice = query.value(name.indexOf("ReceivedPrice")).toDouble();
        double sentQty = query.value(name.indexOf("SentQuantity")).toDouble();
        double recvQty = query.value(name.indexOf("ReceivedQuantity")).toDouble();
        QString clientOrderId = query.value(name.indexOf("ClientOrderId")).toString();
        std::string orderType = query.value(name.indexOf("OrderType")).toString().toStdString();
        double pointCPrice = query.value(name.indexOf("PointCPrice")).toDouble();
        double tickSize = query.value(name.indexOf("TickSize")).toDouble();
        qlonglong strategyId = query.value(name.indexOf("StrategyId")).toLongLong();
        double pipCount = 0.0;
        std::string sideStr = "Buy";
        if (side == OrderMode_BUY) {
            if (orderType == "Primary") {
                sideStr = "Buy";
            }
            else {
                sideStr = "Sell";
            }
        }
        else {
            if (orderType == "Primary") {
                sideStr = "Sell";
            }
            else {
                sideStr = "Buy";
            }
        }

        if (orderType == "Primary") {
            priceMap[strategyId] = recvPrice;
        }
        else {
            if (priceMap.find(strategyId) != priceMap.end()) {
                pipCount = applyTickSize(((side == OrderMode_SELL)?
                                (priceMap[strategyId] - recvPrice) :
                                (recvPrice - priceMap[strategyId])), tickSize);
            }
        }

        myfile << query.value(name.indexOf("UploadName")).toString().toStdString() << ","
               << query.value(name.indexOf("TimeStamp")).toDateTime().toString("dd.MM.yyyy hh:mm:ss.zzz").toStdString() << ","
               << query.value(name.indexOf("Instrument")).toString().toStdString() << ","
               << pipCount << ","
               << query.value(name.indexOf("AlgoType")).toInt() << ","
               << sideStr << ","
               << orderType << ","
               << (orderType == "Primary"?pointCPrice:0) << ","
               << sentPrice << ","
               << recvPrice << ","
               << getSlippage(orderType, side, pointCPrice, sentPrice, recvPrice, tickSize)  << ","
               << sentQty << ","
               << recvQty << ","
               << checkFilledOrderStatus (quantityMap, clientOrderId, sentQty, recvQty) << ","
               << clientOrderId.toStdString() << ","
               << query.value(name.indexOf("UploadId")).toString().toStdString() << ","
               << query.value(name.indexOf("InternalId")).toLongLong() << ","
               << query.value(name.indexOf("StrategyId")).toLongLong() << ","
               << ((side == OrderMode_BUY)? query.value(name.indexOf("BuyAccount")).toString().toStdString()
                              : query.value(name.indexOf("SellAccount")).toString().toStdString()) << "\n";
    }
    myfile.close();
    myfile.clear();
}

double ReportGenerator::getSlippage(std::string &orderType, int &side, double &pointCPrice, double &orderPrice,
                                    double &fillPrice, double &tickSize)
{
    double slippage = 0.0;
    if (orderType == "Primary")
    {
        slippage = (side == OrderMode_SELL)? (pointCPrice - fillPrice) : (fillPrice - pointCPrice);
    }
    else
    {
        slippage = (side == OrderMode_SELL)? (orderPrice - fillPrice) : (fillPrice - orderPrice);
    }
    return applyTickSize(slippage, tickSize);
}

std::string ReportGenerator::checkFilledOrderStatus(std::map<QString, double> &quantityMap,
                                                    QString &clientOrderId, double &sentQty, double &recvQty)
{
    std::string filledStatus = "Fully Filled";
    if (sentQty == recvQty)
    {
        filledStatus = "Fully Filled";
    }
    else if (quantityMap.find(clientOrderId) != quantityMap.end())
    {
        double filledQuantity = quantityMap[clientOrderId];
        if (sentQty == (filledQuantity + recvQty))
        {
            filledStatus = "Fully Filled";
        }
        else
        {
            filledStatus = "Partially Filled";
        }
        quantityMap[clientOrderId] = (filledQuantity + recvQty);
    }
    else
    {
        quantityMap[clientOrderId] = recvQty;
        filledStatus = "Partially Filled";
    }
    return filledStatus;
}

void ReportGenerator::createOpenPositionReport(const QString &applicationPath)
{
    DatabaseConnection &dbConnection = DatabaseConnection::getInstance();
    std::ofstream myfile;

    myfile.open(applicationPath.toStdString());
    if (myfile.is_open())
    {
        myfile << "UploadName, FilledTimeStamp, Instrument, AlgoType, Side, Filled Quantity, Order Quantity, FillPrice, EOPrice,"
                  "Slippage, InternalId, StrategyId, UploadId, PointA, PointB, PointC, CP1, CP2,"
                  "Profit, Loss, Trailing, Tolerance, CancelTicks, Start, End, TExit\n";
    }
    QSqlQuery query = dbConnection.executeQuery(getOpenPositionsFromAGivenDayQuery(_startDate));

    while (query.next()) {
        QSqlRecord name = query.record();
        int side  = (query.value(name.indexOf("Side")).toString() == "BUY"?Algo::OrderMode_BUY:Algo::OrderMode_SELL);
        double eoPrice = query.value(name.indexOf("sentPrice")).toDouble();
        double fillPrice = query.value(name.indexOf("ReceivedPrice")).toDouble();
        double tickSize = query.value(name.indexOf("TickSize")).toDouble();

        myfile << query.value(name.indexOf("UploadName")).toString().toStdString() << ","
               << query.value(name.indexOf("TimeStamp")).toDateTime().toString("dd.MM.yyyy hh:mm:ss.zzz").toStdString() << ","
               << query.value(name.indexOf("Instrument")).toString().toStdString() << ","
               << query.value(name.indexOf("AlgoType")).toInt() << ","
               << ((side == OrderMode_BUY)? "Buy" : "Sell") << ","
               << query.value(name.indexOf("ReceivedQuantity")).toDouble() << ","
               << query.value(name.indexOf("SentQuantity")).toDouble() << ","
               << fillPrice << ","
               << eoPrice << ","
               << applyTickSize(((side == OrderMode_BUY)? (eoPrice - fillPrice):(fillPrice - eoPrice)), tickSize)<< ","
               << query.value(name.indexOf("InternalId")).toLongLong() << ","
               << query.value(name.indexOf("StrategyId")).toLongLong() << ","
               << query.value(name.indexOf("UploadId")).toString().toStdString() << ","
               << query.value(name.indexOf("PointAPercentage")).toDouble() << ","
               << query.value(name.indexOf("PointBPercentage")).toDouble() << ","
               << query.value(name.indexOf("PointCPercentage")).toDouble() << ","
               << query.value(name.indexOf("CP1Percentage")).toDouble() << ","
               << query.value(name.indexOf("CP2Percentage")).toDouble() << ","
               << query.value(name.indexOf("ProfitPercentage")).toDouble() << ","
               << query.value(name.indexOf("LossPercentage")).toDouble() << ","
               << query.value(name.indexOf("TrailingPercentage")).toDouble() << ","
               << query.value(name.indexOf("Tolerance")).toDouble() << ","
               << query.value(name.indexOf("CancelTicks")).toDouble() << ","
               << query.value(name.indexOf("StartDate")).toDate().toString("dd.MM.yyyy").toStdString() + " " + query.value(name.indexOf("StartTime")).toTime().toString("hh:mm:ss").toStdString() << ","
               << query.value(name.indexOf("EndDate")).toDate().toString("dd.MM.yyyy").toStdString() + " " + query.value(name.indexOf("EndTime")).toTime().toString("hh:mm:ss").toStdString() << ","
               << query.value(name.indexOf("TExitDate")).toDate().toString("dd.MM.yyyy").toStdString() + " " + query.value(name.indexOf("TExitTime")).toTime().toString("hh:mm:ss").toStdString() << "\n";
        myfile << ","
               << ","
               << ","
               << ","
               << ","
               << ","
               << ","
               << ","
               << ","
               << ","
               << ","
               << ","
               << ","
               << query.value(name.indexOf("PointAPrice")).toDouble() << ","
               << query.value(name.indexOf("PointBPrice")).toDouble() << ","
               << query.value(name.indexOf("PointCPrice")).toDouble() << ","
               << query.value(name.indexOf("CP1Price")).toDouble() << ","
               << query.value(name.indexOf("CP2Price")).toDouble() << ","
               << query.value(name.indexOf("Profit")).toDouble() << ","
               << query.value(name.indexOf("Loss")).toDouble() << ","
               << query.value(name.indexOf("Trailing")).toDouble() << "\n";
    }
    myfile.close();
    myfile.clear();
}

void ReportGenerator::createClosedStrategyReport(const QString &applicationPath)
{
    DatabaseConnection &dbConnection = DatabaseConnection::getInstance();
    std::ofstream myfile;

    myfile.open(applicationPath.toStdString());
    if (myfile.is_open())
    {
        myfile << "UploadName, StrategyId, InternalId, UploadId, Instrument, AlgoType, Side, Quantity, "
                  "PrimaryOrderPrice, PrimaryFilledPrice, PrimarySlippage, PrimaryFilledQuantity, "
                  "SecondaryOrderPrice, SecondaryFilledPrice, SecondarySlippage, SecondaryFilledQuantity, "
                  "PipCount,Pnl\n";
    }

    QSqlQuery query = dbConnection.executeQuery(getAllClosedStrategiesFromAGivenDayQuery(_startDate));

    while (query.next()) {
        bool isChildFilledPresent = false;
        QSqlRecord name = query.record();
        int side  = (query.value(name.indexOf("Side")).toString() == "BUY"?Algo::OrderMode_BUY:Algo::OrderMode_SELL);
        double tickSize = query.value(name.indexOf("TickSize")).toDouble();

        QSqlQuery orderQuery = dbConnection.executeQuery(getAllFilledOrdersForAGivenStrategy(
                                                             query.value(name.indexOf("StrategyId")).toString()));
        double  poSentPrice=0.0, poSentQuantity=0,
                poRecvPrice=0.0, poRecvQuantity=0,
                soSentPrice=0.0, soSentQuantity=0,
                soRecvPrice=0.0, soRecvQuantity=0;
        double pnl=0;
        double buyPrice=0, sellPrice=0;
        long buyQty=0, sellQty=0;

        while (orderQuery.next()) {
            QSqlRecord record = orderQuery.record();
            QString orderType = orderQuery.value(record.indexOf("OrderType")).toString();
            QString filledOrderSide = orderQuery.value(record.indexOf("Side")).toString();
            double filledPrice = 0.0, filledQuantity = 0.0;

            filledPrice = orderQuery.value(record.indexOf("ReceivedPrice")).toDouble();
            filledQuantity = orderQuery.value(record.indexOf("ReceivedQuantity")).toDouble();

            if(filledOrderSide == "BUY") {
                buyPrice += filledPrice * filledQuantity;
                buyQty += filledQuantity;
            }
            else {
                sellPrice += filledPrice * filledQuantity;
                sellQty += filledQuantity;
            }

            if (orderType == "Primary") {
                poSentPrice = orderQuery.value(record.indexOf("sentPrice")).toDouble();
                poSentQuantity = orderQuery.value(record.indexOf("SentQuantity")).toDouble();

                poRecvPrice = filledPrice;
                poRecvQuantity = filledQuantity;
            }
            else {
                soSentPrice = orderQuery.value(record.indexOf("sentPrice")).toDouble();
                soSentQuantity = orderQuery.value(record.indexOf("SentQuantity")).toDouble();

                soRecvPrice = filledPrice;
                soRecvQuantity = filledQuantity;
                isChildFilledPresent = true;
            }
        }

        if(sellQty>0)
        {
            pnl = sellPrice/sellQty;
        }
        if(buyQty>0)
        {
            pnl -= buyPrice/buyQty;
        }
        if(buyQty==0 || sellQty ==0) {
            pnl = 0.0;
        }

        if (isChildFilledPresent == true) {
            pnl =applyTickSize(pnl,  tickSize);
            myfile << query.value(name.indexOf("UploadName")).toString().toStdString() << ","
                   << query.value(name.indexOf("StrategyId")).toLongLong() << ","
                   << query.value(name.indexOf("InternalId")).toLongLong() << ","
                   << query.value(name.indexOf("UploadId")).toString().toStdString() << ","
                   << query.value(name.indexOf("Instrument")).toString().toStdString() << ","
                   << query.value(name.indexOf("AlgoType")).toInt() << ","
                   << ((side == OrderMode_BUY)? "Buy" : "Sell") << ","
                   << query.value(name.indexOf("Quantity")).toInt() << ",";
            myfile << poSentPrice << ","
                   << poRecvPrice << ","
                   << applyTickSize(((side == OrderMode_BUY)? (poRecvPrice - poSentPrice):(poSentPrice - poRecvPrice)), tickSize) << ","
                   << poRecvQuantity << ","
                   << soSentPrice << ","
                   << soRecvPrice << ","
                   << applyTickSize(((side == OrderMode_BUY)? (soRecvPrice - soSentPrice):(soSentPrice - soRecvPrice)), tickSize) << ","
                   << soRecvQuantity << ","
                   << ((soRecvQuantity == 0)?0:applyTickSize(((side == 1)? (soRecvPrice - poRecvPrice):(poRecvPrice - soRecvPrice)), tickSize)) << ","
                   << pnl << "," << "\n";
        }

    }
    myfile.close();
    myfile.clear();
}

double ReportGenerator::applyTickSize(double price, double ticksize)
{
    if (Settings::shouldUseTickSize()) {
        return (price / ticksize);
    }
    return price;
}

}
