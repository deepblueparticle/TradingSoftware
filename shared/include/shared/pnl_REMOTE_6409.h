#ifndef PNL_H
#define PNL_H

#include "order.h"
#include <map>
#include <list>
#include <vector>

#include <boost/noncopyable.hpp>

namespace Algo {

static const std::vector<std::string> InstrumentsName = {"AUD/CHF","AUD/JPY","AUD/NZD","AUD/USD",
                                       "CAD/CHF","CAD/JPY","CHF/JPY","EUR/AUD",
                                       "EUR/CAD","EUR/CHF","EUR/CZK","EUR/GBP",
                                       "EUR/JPY","EUR/NOK","EUR/NZD","EUR/SEK",
                                       "EUR/TRY","EUR/USD","GBP/AUD","GBP/CAD",
                                       "GBP/CHF","GBP/JPY","GBP/NZD","GBP/USD",
                                       "NZD/CAD","NZD/CHF","NZD/JPY","NZD/USD",
                                       "TRY/JPY","USD/CAD","USD/CHF","USD/CZK",
                                       "USD/HKD","USD/JPY","USD/MXN","USD/NOK",
                                       "USD/SEK","USD/TRY","USD/ZAR","ZAR/JPY"};

static const double TickSize[]={0.00001,0.001,0.00001,0.00001,
                               0.00001,0.001,0.001,0.00001,
                               0.00001,0.00001,0.0001,0.00001,
                               0.001,0.00001,0.00001,0.00001,
                               0.00001,0.00001,0.00001,0.00001,
                               0.00001,0.001,0.00001,0.00001,
                               0.00001,0.00001,0.001,0.00001,
                               0.001,0.00001,0.00001,0.0001,
                               0.00001,0.001,0.00001,0.00001,
                               0.00001,0.00001,0.00001,0.001};

class PnL : public boost::noncopyable
{
public:
    static PnL& GetInstance()
    {
        static PnL p;
        return p;
    }
    void InsertExecReport(long strategyId, Order ord)
    {
        m_mStrategyOrderMap[strategyId].push_back(ord);
    }
    double GetPnL(long strategyId, double& avgBuyPrice, double& avgSellPrice);
private:
    std::map<long, std::list<Order>> m_mStrategyOrderMap;
};
}
#endif // PNL_H
