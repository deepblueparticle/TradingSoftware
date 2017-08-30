#ifndef Nanex_MarketDataHandler_H
#define Nanex_MarketDataHandler_H

#include <string>
#include <unordered_map>
#include <mutex>
#include <map>

#include <shared/imarket_data_handler.h>
#include <nanex_api/NxCoreAPI_Wrapper_C++.h>

namespace Algo
{
namespace Nanex
{

struct TimeRange
{
  unsigned long startMilliSeconds;
  unsigned long endMilliSeconds;
};

/*! \class    MarketDataHandler
  * \brief    Handler to access market data from Nanex
  */
class MarketDataHandler : public Algo::IMarketDataHandler
{
  public:
    /// Get singleton instance
    static MarketDataHandler& getInstance() {
      static MarketDataHandler instance;
      return instance;
    }

    virtual ~MarketDataHandler();

    /*!
     * \brief It checks if connection is still alive or not
     */
    bool isConnected() override;

    void loadSymbols(std::string filename);
    void loadPreTradingHours(std::string filename);
    bool isRealTime(const NxCoreSystem* pNxCoreSys);
    bool isUnderPreTradingHours(const NxCoreSystem* pNxCoreSys);

    void onNxCoreStatus(const NxCoreSystem* pNxCoreSys,const NxCoreMessage* pNxCoreMsg);
    static int onNxCoreCallback(const NxCoreSystem* pNxCoreSys,const NxCoreMessage* pNxCoreMsg);

    void processMessages();

  protected:
    /*!
     * \brief Subscribe to market data to given symbol.
     *
     * \param symbol Symbol to be subscribed.
     *
     * \return True if connected successfully, otherwise false.
     */
    virtual bool subscribeImplementation(const std::string& symbol);

    /*!
     * \brief Unsubscribe from market data to given symbol.
     *
     * \param symbol Symbol to be unsubscribed.
     *
     * \return True if connected successfully, otherwise false.
     */
    virtual bool unsubscribeImplementation(const std::string& symbol);

    /*!
     * \brief Unsubscribe from market data to given symbol.
     *
     * \param pNxCoreSys NxCoreSystem object contains status, userdata
     *
     * \param pNxCoreMsg This contains tickdata
     */
    void onGetExgQuoteStates(const NxCoreSystem* pNxCoreSys, const NxCoreMessage* pNxCoreMsg);

  private:
    MarketDataHandler();
    MarketDataHandler(const MarketDataHandler&);

    std::mutex _mutex;
    bool _isFeedConnected;
    NxCoreClass _nxCore;
    bool _isRealTime;
    std::map<std::string, std::string> _symbolList;
    std::map<int, TimeRange> _preTradingHours;
};

} // namespace Nanex
} // namespace Algo

#endif
