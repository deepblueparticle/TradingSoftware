#include <shared/pnl.h>
#include <algorithm>

namespace Algo {

double PnL::GetPnL(long strategyId, double& avgBuyPrice, double& avgSellPrice)
{
    double pnl=0;
    double buyPrice=0;
    double sellPrice=0;
    long buyQty=0;
    long sellQty=0;

    auto itr = m_mStrategyOrderMap.find(strategyId);
    std::string instrument;
    if(itr != m_mStrategyOrderMap.end())
    {
        for(auto ordItr : itr->second)
        {
            if(ordItr.getOrderMode() == OrderMode_BUY)
            {
                buyPrice += ordItr.getFilledPrice() * ordItr.getFilledQuantity();
                buyQty += ordItr.getFilledQuantity();
            }
            else
            {
                sellPrice += ordItr.getFilledPrice() * ordItr.getFilledQuantity();
                sellQty += ordItr.getFilledQuantity();
            }
            instrument = ordItr.getSymbol();
        }
    }
    if(sellQty>0)
    {
        pnl = sellPrice/sellQty;
        avgSellPrice = sellPrice/sellQty;
    }
    if(buyQty>0)
    {
        pnl -= buyPrice/buyQty;
        avgBuyPrice = buyPrice/buyQty;
    }

    if(buyQty==0 || sellQty ==0)
        return 0;
    auto it = std::find(InstrumentsName.begin(), InstrumentsName.end(), instrument);
    if(it != InstrumentsName.end())
    {
        double ticks=TickSize[std::distance(InstrumentsName.begin(), it)];
        pnl = pnl/(10*ticks);
    }

    return pnl;
}


}
