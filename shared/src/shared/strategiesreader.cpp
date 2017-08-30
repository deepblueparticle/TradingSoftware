#include <shared/strategiesreader.h>
#include "algomodel1.h"

#include <QtXlsx/xlsxdocument.h>
#include <QDebug>
#include <QDateTime>

namespace Algo {


bool getValue(QXlsx::Document &xlsx, int row, int col, QVariant &val, QVariant::Type type)
{
    QXlsx::Cell *cell=xlsx.cellAt(col,row);

    if (cell) {
        if(cell->value().type()!= type) {
            try{
                if(cell->value().toString().toStdString()[0] == '#')
                {
                    qDebug() << "Data missing in cell." ;
                    return false;
                }
                switch (type){
                    case QVariant::Double:
                        val = cell->value().toDouble();
                        break;
                    case QVariant::String:
                        val = cell->value().toString();
                }
                return true;
            } catch(...){
                qDebug() << "Could not change type";
                return false;
            }

            return false;
        }
        else
            val=cell->value();
    }
    else {
        return false;
    }

    return true;
}

bool readDateTime(QXlsx::Document &xlsx, int row, int col, std::string& val, bool isTime=false)
{
    if (QXlsx::Cell *cell=xlsx.cellAt(col, row))
    {
        QDateTime qdt = cell->dateTime();
        QString format("MM/dd/yyyy");
        if(isTime)
        {
            format = QString("hh:mm:ss");
        }
        val = qdt.toString(format).toStdString();
        return true;
    }
    return false;
}

void getStrategiesFromExcel(const std::string &excelFileName, InsertAlgo insertAlgo, int &strategyAdded, std::string upname)
{
    std::map<std::string, SymbolData> symbolMap;
    QXlsx::Document xlsx(excelFileName.c_str());

    xlsx.selectSheet("SymbolFactory");
    int row = 2;
    while(true)
    {
        SymbolData symbol;
        QVariant data;
        if(!getValue(xlsx, 2, row, data, QVariant::String))
            break;
        symbol.secType = data.toString().toStdString();
        if(!getValue(xlsx, 3, row, data, QVariant::String))
            break;
        symbol.exchange = data.toString().toStdString();
        if(!getValue(xlsx, 4, row, data, QVariant::String))
            break;
        symbol.currency = data.toString().toStdString();
        if(!getValue(xlsx, 1, row, data, QVariant::String) || (data.toString() == ""))
            break;
        symbolMap[data.toString().toStdString()] = symbol;
        row++;
    }
    xlsx.selectSheet("Strategy");

    int col = 2;
    strategyAdded = 0;
    while (true)
    {
        std::shared_ptr<algomodel1> am(new algomodel1());
        am->m_upname = upname;
        QVariant data;
        if(!getValue(xlsx, 2, col, data, QVariant::Double))
                break;
        am->m_tickSize = data.toDouble();

        if(!getValue(xlsx, 3, col, data, QVariant::Double))
                break;
        am->m_algoType = (AlgoType)(int)(data.toDouble());

        if(!getValue(xlsx, 4, col, data, QVariant::String))
                break;
        if(data.toString()== "BUY")
            am->m_mode = OrderMode_BUY;
        else
            am->m_mode = OrderMode_SELL;

        if(!getValue(xlsx, 5, col, data, QVariant::Double))
                break;
        am->m_prevClose = data.toDouble();

        if(!getValue(xlsx, 6, col, data, QVariant::Double))
                break;
        am->m_baseVal = data.toDouble();


        if(!getValue(xlsx, 7, col, data, QVariant::Double))
                break;
        am->m_pointAPercentage = data.toDouble()*100;

        if(!getValue(xlsx, 8, col, data, QVariant::Double))
                break;
        am->m_pointBPercentage = data.toDouble()*100;

        if(!getValue(xlsx, 9, col, data, QVariant::Double))
                break;
        am->m_entryPrice = data.toDouble();

        if(!getValue(xlsx, 10, col, data, QVariant::Double))
                break;
        am->m_cp1Percentage = data.toDouble()*100;

        if(!getValue(xlsx, 11, col, data, QVariant::Double))
                break;
        am->m_cp2Percentage = data.toDouble()*100;

        if(!getValue(xlsx, 13, col, data, QVariant::Double))
                break;
        am->m_profitPercentage = data.toDouble()*100;

        if(!getValue(xlsx, 14, col, data, QVariant::Double))
                break;
        am->m_lossPercentage = data.toDouble() * 100;

        if(!getValue(xlsx, 16, col, data, QVariant::Double))
                break;
        am->m_qty = data.toDouble();

        if(!getValue(xlsx, 17, col, data, QVariant::Double))
                break;
        am->m_cancelTicks = data.toDouble();

        if(!getValue(xlsx, 18, col, data, QVariant::Double))
                break;
        am->m_tolerance = data.toDouble();

        if(!getValue(xlsx, 19, col, data, QVariant::Double))
            break;
        am->m_maxQty = data.toDouble();


        if(!getValue(xlsx, 20, col, data, QVariant::Double))
                break;
        am->m_timeEnabled = data.toString().toDouble();

        std::string datetime;
        if(!readDateTime(xlsx, 21, col, datetime))
                break;
        am->m_startDate = datetime;

        if(!readDateTime(xlsx, 22, col, datetime, true))
                break;
        am->m_startTime = datetime;


        if(!readDateTime(xlsx, 23, col, datetime))
                break;
        am->m_endDate = datetime;

        if(!readDateTime(xlsx, 24, col, datetime, true))
                break;
        am->m_endTime = datetime;

        if(!getValue(xlsx, 25, col, data, QVariant::Double))
                break;
        am->m_trailingPercentage = data.toDouble();

        if(!getValue(xlsx, 21, col+1, data, QVariant::Double))
                break;
        am->m_startDateEpoch = data.toDouble();

        if(!getValue(xlsx, 22, col+1, data, QVariant::Double))
                break;
        am->m_startTimeEpoch = data.toDouble();


        if(!getValue(xlsx, 23, col+1, data, QVariant::Double))
                break;
        am->m_endDateEpoch = data.toDouble();

        if(!getValue(xlsx, 24, col+1, data, QVariant::Double))
                break;
        am->m_endTimeEpoch = data.toDouble();

        //Daily/Weekly
        if(!getValue(xlsx, 26, col, data, QVariant::String))
                break;
        am->m_dailyOrWeekly = data.toString().toStdString();
        am->m_identifier = data.toString().toStdString();

        if (!readDateTime(xlsx, 27, col, datetime))
            break;
        am->m_secondaryOrderExitDate = datetime;

        if (!readDateTime(xlsx, 28, col, datetime, true))
            break;
        am->m_secondaryOrderExitTime = datetime;

        if (!getValue(xlsx, 27, col + 1, data, QVariant::Double))
            break;
        am->m_secondaryOrderExitDateEpoch = data.toDouble();

        if (!getValue(xlsx, 28, col + 1, data, QVariant::Double))
            break;
        am->m_secondaryOrderExitTimeEpoch = data.toDouble();

        if (!getValue(xlsx, 29, col + 1, data, QVariant::Double))
            break;
        am->m_secondaryOrderExitEnabled = data.toDouble();

        if(!getValue(xlsx, 1, col+1, data, QVariant::String))
                break;
        //Extra check if for instrument in excel.
        if(data.toString().toStdString()=="")
            break;
        am->dataSymbol = data.toString().toStdString();
        SymbolData symbolData = symbolMap[data.toString().toStdString()];
        am->dataSecType = symbolData.secType;
        am->dataExchange = symbolData.exchange;
        am->dataCurrency = symbolData.currency;

        if(!getValue(xlsx, 7, col+1, data, QVariant::Double))
            break;
        am->m_pointACalculated = data.toDouble();

        if(!getValue(xlsx, 8, col+1, data, QVariant::Double))
            break;
        am->m_pointBCalculated = data.toDouble();

        if(!getValue(xlsx, 9, col+1, data, QVariant::Double))
            break;
        am->m_entryPriceCalculated = data.toDouble();

        if(!getValue(xlsx, 10, col+1, data, QVariant::Double))
            break;
        am->m_cp1Calculated = data.toDouble();

        if(!getValue(xlsx, 11, col+1, data, QVariant::Double))
            break;
        am->m_cp2Calculated = data.toDouble();

        if(!getValue(xlsx, 12, col+1, data, QVariant::Double))
                break;
        am->m_parentOrderPrice = data.toDouble();

        if(!getValue(xlsx, 13, col+1, data, QVariant::Double))
            break;
        am->m_profitCalculated = data.toDouble();

        if(!getValue(xlsx, 14, col+1, data, QVariant::Double))
            break;
        am->m_lossCalculated = data.toDouble();

        if(!getValue(xlsx, 15, col+1, data, QVariant::Double))
                break;
        am->m_lossLimitPrice = data.toDouble();

        if(!getValue(xlsx, 25, col+1, data, QVariant::Double))
                break;
        am->m_trailing = data.toDouble();

        am->m_internalId = getUniqueId();
        am->m_status = StrategyStatus_START;
        am->m_strategyId = 0;

        insertAlgo(am);
        ++strategyAdded;
        col += 2;
    }
}

}
