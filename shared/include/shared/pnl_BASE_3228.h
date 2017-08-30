#ifndef PNL_H
#define PNL_H

#include "order.h"
#include <map>
#include <list>

namespace Algo {

class PnL
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
    double GetPnL(long strategyId);
private:
    PnL(){}
    std::map<long, std::list<Order>> m_mStrategyOrderMap;
};
}
#endif // PNL_H
