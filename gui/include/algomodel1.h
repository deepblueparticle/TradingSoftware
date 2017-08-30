#ifndef ALGOMODEL1_H
#define ALGOMODEL1_H
#include <QDebug>
#include <string>
#include <map>
#include <memory>
#include <shared/commands.h>

namespace  Algo {

const QString Algomodel1Header =
"StrategyId,InternalId,TickSize,Mode,PrevClose,BaseVal,EntryPrice,EntryPriceCalculated,PointAPercentage,\
PointACalculated,PointBPercentage,PointBCalculated,Cp1Percentage,Cp1Calculated,Cp2Percentage,Cp2Calculated,\
ProfitPercentage,ProfitCalculated,LossPercentage,LossCalculated,Tolerance,TimeEnabled,StartDate,StartTime,\
EndDate,EndTime,Status,Qty,MaxQty,\
PointATime,PointBTime,CancelTime,InstrumentName,CancelTicks,CPA,CPB\n";

struct algomodel1
{
    algomodel1();

    long            m_strategyId;
    long            m_internalId;
    std::string     dataSymbol;
    std::string     dataSecType;
    std::string     dataExchange;
    std::string     dataCurrency;
    double           m_tickSize;
    OrderMode       m_mode;
    double          m_prevClose;
    double          m_baseVal;
    double          m_entryPrice;
    double          m_entryPriceCalculated;
    double          m_trailingPercentage;
    double          m_pointAPercentage;
    double          m_pointACalculated;
    double          m_pointBPercentage;
    double          m_pointBCalculated;
    double          m_cp1Percentage;
    double          m_cp1Calculated;
    double          m_cp2Percentage;
    double          m_cp2Calculated;
    double          m_parentOrderPrice;
    double          m_profitPercentage;
    double          m_profitCalculated;
    double          m_lossPercentage;
    double          m_lossCalculated;
    double          m_lossLimitPrice;
    double          m_tolerance;
    UNSIGNED_INTEGER    m_timeEnabled;
    std::string     m_startTime;
    std::string     m_endTime;
    std::string     m_startDate;
    std::string     m_endDate;

    long long     m_startTimeEpoch;
    long long     m_endTimeEpoch;
    long long     m_startDateEpoch;
    long long     m_endDateEpoch;

    // adding T-exit parameter
    long long     m_secondaryOrderExitDateEpoch;
    long long     m_secondaryOrderExitTimeEpoch;
    bool          m_secondaryOrderExitEnabled;
    std::string   m_secondaryOrderExitDate;
    std::string   m_secondaryOrderExitTime;

    StrategyStatus  m_status;
    long long       m_qty;
    long long       m_maxQty;
    std::string     m_pointATime;
    std::string     m_pointBTime;
    std::string     m_cancelTime;
    long long       m_cancelTicks;
    std::string     m_cpATime;
    std::string     m_cpBTime;
    double          m_trailing;
    AlgoType        m_algoType;
    std::string     m_dailyOrWeekly;
    std::string     m_upname;
    std::string     m_identifier;
    double          m_pnl;

    void toAlgoModel1(AlgoModel1 *algoModel1);
    void fromAlgoModel1(AlgoModel1* algoModel1);


public:
    double qty() const;
    void setQty(double qty);
    double maxQty() const;
    void setMaxQty(double maxQty);
};
class algomodelStrategy1
{
public:
   static algomodelStrategy1& getInstance()
   {
       static algomodelStrategy1 a;
       return a;
   }

   std::shared_ptr<algomodel1> getStrategy(long id)
   {
       return m_idStrategyMap[id];
   }

   void addStrategy(long id, std::shared_ptr<algomodel1> algo)
   {
       qDebug() << "Adding strategy [" << id <<"] in the map.";
       qDebug() << "Qty = "<<algo->qty();
       qDebug() << "Max Qty = "<<algo->maxQty();
       m_idStrategyMap[id] = algo;
   }

   void updateStrategy(long id, std::shared_ptr<algomodel1> algo)
   {
       m_idStrategyMap[id] = algo;
   }

   std::map<long, std::shared_ptr<algomodel1>>& getIdStrategyMap()
   {
       return m_idStrategyMap;
   }

private:
    algomodelStrategy1() {}
    std::map<long, std::shared_ptr<algomodel1>> m_idStrategyMap;
};

}
#endif // ALGOMODEL1_H
