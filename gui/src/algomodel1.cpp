#include <QObject>
#include <shared/client_utils.h>
#include <sstream>
#include "algomodel1.h"
#include "symboltickmap.h"

namespace Algo {


algomodel1::algomodel1():m_pnl(0.0)
{}
double algomodel1::maxQty() const
{
    return m_maxQty;
}

void algomodel1::setMaxQty(double maxQty)
{
    m_maxQty = maxQty;
}

double algomodel1::qty() const
{
    return m_qty;
}

void algomodel1::setQty(double qty)
{
    m_qty = qty;
}



void algomodel1::toAlgoModel1(AlgoModel1* algoModel1)
{
    algoModel1->SetStrategyId(m_strategyId);
    algoModel1->SetInternalId(m_internalId);
    algoModel1->SetDataSymbol(dataSymbol.c_str());
    algoModel1->SetDataSecType(dataSecType.c_str());
    algoModel1->SetDataExchange(dataExchange.c_str());
    algoModel1->SetDataCurrency(dataCurrency.c_str());
    algoModel1->SetTickSize(m_tickSize);
    algoModel1->SetMode((UNSIGNED_CHARACTER)m_mode);
    algoModel1->SetBaseValue(m_baseVal);
    algoModel1->SetPrevClose(m_prevClose);
    algoModel1->SetEntryPrice(m_entryPrice);
    algoModel1->SetEntryPriceCalc(m_entryPriceCalculated);
    algoModel1->SetPointA(m_pointAPercentage);
    algoModel1->SetPointACalc(m_pointACalculated);
    algoModel1->SetPointB(m_pointBPercentage);
    algoModel1->SetPointBCalc(m_pointBCalculated);

    algoModel1->SetCancelPoint1(m_cp1Percentage);
    algoModel1->SetCancelPoint1Calc(m_cp1Calculated);
    algoModel1->SetCancelPoint2(m_cp2Percentage);
    algoModel1->SetCancelPoint2Calc(m_cp2Calculated);

    algoModel1->setParentOrderPrice(m_parentOrderPrice);
    algoModel1->SetProfit(m_profitPercentage);
    algoModel1->SetProfitCalc(m_profitCalculated);
    algoModel1->SetLoss(m_lossPercentage);
    algoModel1->SetLossCalc(m_lossCalculated);
    algoModel1->SetLossLimitPrice(m_lossLimitPrice);

    algoModel1->SetTolerance(m_tolerance);

    algoModel1->SetTimeEnabled(m_timeEnabled);
    algoModel1->SetStartDate(m_startDate.c_str());
    algoModel1->SetEndDate(m_endDate.c_str());
    algoModel1->SetStartTime(m_startTime.c_str());
    algoModel1->SetEndTime(m_endTime.c_str());

    algoModel1->setSecondaryOrderExitDateEpoch(m_secondaryOrderExitDateEpoch);
    algoModel1->setSecondaryOrderExitTimeEpoch(m_secondaryOrderExitTimeEpoch);
    algoModel1->setSecondaryOrderExitDate(m_secondaryOrderExitDate.c_str());
    algoModel1->setSecondaryOrderExitTime(m_secondaryOrderExitTime.c_str());
    algoModel1->setSecondaryOrderExitEnabled(m_secondaryOrderExitEnabled);

    algoModel1->SetStartDateEpoch(m_startDateEpoch);
    algoModel1->SetEndDateEpoch(m_endDateEpoch);
    algoModel1->SetStartTimeEpoch(m_startTimeEpoch);
    algoModel1->SetEndTimeEpoch(m_endTimeEpoch);

    algoModel1->SetStatus((UNSIGNED_CHARACTER)m_status);

    algoModel1->SetQty(m_qty);
    algoModel1->SetMaxQty(m_maxQty);

    algoModel1->SetPointATime(m_pointATime.c_str());
    algoModel1->SetPointBTime(m_pointBTime.c_str());
    algoModel1->SetCancelTime(m_cancelTime.c_str());

    algoModel1->SetCancelTicks(m_cancelTicks);
    algoModel1->SetCPATime(m_cpATime.c_str());
    algoModel1->SetCPBTime(m_cpBTime.c_str());
    algoModel1->SetTrailing(m_trailing);
    algoModel1->SetTrailingPercentage(m_trailingPercentage);
    algoModel1->SetAlgoType(m_algoType);
    algoModel1->SetUserName("");
    algoModel1->SetDailyOrWeekly(m_dailyOrWeekly.c_str());
    algoModel1->SetIdentifier(m_identifier.c_str());
    algoModel1->SetUpname(m_upname.c_str());
    algoModel1->SetIdentifier(m_dailyOrWeekly.c_str());
}

void algomodel1::fromAlgoModel1(AlgoModel1* algoModel1)
{
    this->m_strategyId = algoModel1->GetStrategyId();
    this->m_internalId = algoModel1->GetInternalId();
    this->dataSymbol =  std::string(algoModel1->GetDataSymbol());
    this->dataSecType =  std::string(algoModel1->GetDataSecType());
    this->dataExchange =  std::string(algoModel1->GetDataExchange());
    this->dataCurrency =  std::string(algoModel1->GetDataCurrency());
    this->m_tickSize = algoModel1->GetTickSize();
    this->m_mode = (OrderMode)algoModel1->GetMode();
    this->m_baseVal = algoModel1->GetBaseValue();
    this->m_prevClose = algoModel1->GetPrevClose();
    this->m_entryPrice = algoModel1->GetEntryPrice();
    this->m_entryPriceCalculated = algoModel1->GetEntryPriceCalc();
    this->m_pointAPercentage = algoModel1->GetPointA();
    this->m_pointACalculated = algoModel1->GetPointACalc();
    this->m_pointBPercentage = algoModel1->GetPointB();
    this->m_pointBCalculated = algoModel1->GetPointBCalc();
    this->m_cp1Percentage = algoModel1->GetCancelPoint1();
    this->m_cp1Calculated = algoModel1->GetCancelPoint1Calc();
    this->m_cp2Percentage = algoModel1->GetCancelPoint2();
    this->m_cp2Calculated = algoModel1->GetCancelPoint2Calc();

    this->m_parentOrderPrice = algoModel1->getParentOrderPrice();
    this->m_profitPercentage = algoModel1->GetProfit();
    this->m_profitCalculated = algoModel1->GetProfitCalc();
    this->m_lossPercentage = algoModel1->GetLoss();
    this->m_lossCalculated = algoModel1->GetLossCalc();
    this->m_lossLimitPrice = algoModel1->GetLossLimitPrice();

    this->m_tolerance =algoModel1->GetTolerance();
    this->m_timeEnabled = algoModel1->GetIsTimeEnabled();
    this->m_startDate = std::string(algoModel1->GetStartDate());
    this->m_endDate = std::string(algoModel1->GetEndDate());
    this->m_startTime = std::string(algoModel1->GetStartTime());
    this->m_endTime = std::string(algoModel1->GetEndTime());
    this->m_status = (StrategyStatus)algoModel1->GetStatus();

    this->m_startDateEpoch = algoModel1->GetStartDateEpoch();
    this->m_endDateEpoch = algoModel1->GetEndDateEpoch();
    this->m_startTimeEpoch = algoModel1->GetStartTimeEpoch();
    this->m_endTimeEpoch = algoModel1->GetEndTimeEpoch();

    this->m_secondaryOrderExitDateEpoch = algoModel1->getSecondaryOrderExitDateEpoch();
    this->m_secondaryOrderExitTimeEpoch = algoModel1->getSecondaryOrderExitTimeEpoch();
    this->m_secondaryOrderExitDate = algoModel1->getSecondaryOrderExitDate();
    this->m_secondaryOrderExitTime = algoModel1->getSecondaryOrderExitTime();
    this->m_secondaryOrderExitEnabled = algoModel1->getSecondaryOrderExitEnabled();

    this->m_qty = algoModel1->GetQty();
    this->m_maxQty = algoModel1->GetMaxQty();

    this->m_pointATime = std::string(algoModel1->GetPointATime());
    this->m_pointBTime = std::string(algoModel1->GetPointBTime());
    this->m_cancelTime = std::string(algoModel1->GetCancelTime());

    this->m_cancelTicks = algoModel1->GetCancelTicks();
    this->m_cpATime = algoModel1->GetCPATime();
    this->m_cpBTime = algoModel1->GetCPBTime();
    this->m_trailing = algoModel1->GetTrailingStop();
    this->m_trailingPercentage = algoModel1->getTrailingPercentage();
    this->m_algoType = (AlgoType)algoModel1->GetAlgoType();
    this->m_dailyOrWeekly = algoModel1->GetDailyOrWeekly();
    this->m_identifier = algoModel1->GetIdentifier();
    this->m_upname = algoModel1->GetUpname();
    this->m_pnl = 0.0;
}

}
