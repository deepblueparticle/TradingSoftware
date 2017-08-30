#ifndef Ib_MarketDataHandler_H
#define Ib_MarketDataHandler_H

#include <string>
#include <unordered_map>
#include <mutex>

#include <interactive_brokers/ConnectionHandler.H>
#include <interactive_brokers/ContractsFactory.H>

#include <ib_api/client/Contract.h>
#include <shared/imarket_data_handler.h>

namespace Algo
{
namespace Ib
{

/*! \class    MarketDataHandler
  * \brief    Handler to access market data from IB
  */
class MarketDataHandler : public Ib::ConnectionHandler,
                          public Algo::IMarketDataHandler
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

    /*!
     * \brief Connect to TWS or IB Gateway API connection.
     *
     * \param host IP or hostname of the computer running IB Gateway instance.
     * \param port Port of the computer running IB Gateway instance.
     *
     * \return True if connected successfully, otherwise false.
     */
    virtual bool connect(const std::string& host, const int port) override;

  protected:
    /*!
     * \brief Subscribe to market data to given symbol (IB contract).
     *
     * \param contract A string code of Contract. For supported values:
     *                 \see ContractsFactory::create().
     *
     * \return True if connected successfully, otherwise false.
     */
    virtual bool subscribeImplementation(const std::string& contract);

    /*!
     * \brief Unsubscribe from market data to given symbol (IB contract).
     *
     * \param contract A string code of Contract. For supported values:
     *                 \see ContractsFactory::create().
     *
     * \return True if connected successfully, otherwise false.
     */
    virtual bool unsubscribeImplementation(const std::string& contract);

  private:
    MarketDataHandler();
    MarketDataHandler(const MarketDataHandler&);

    // EWrapper methods (IB API callbacks)

    /*!
     * \brief IB API callback that handles ticker price data. Handles only
     *        ASK and BID prices, all other ticker types are skiped.
     *        ASK and BID prices pushes to QuoteBook class.
     *
     * \see QuoteBook
     */
    void tickPrice(TickerId tickerId, TickType field, double price,
                   int canAutoExecute) override;

    /*!
     * \brief IB API callback that handles error cases
     * \param id Id of the message
     * \param errorCode ErrorCode of the scenario that has happened
     * \param errorStr Detailed description of the error
     */
    virtual void error(const int id, const int errorCode,
                       const std::string errorStr) override;

    /*!
     * \brief Calls IB Api to resubscribe all previously subscribed symbols
     */
    void reSubscribeSymbols();

    std::mutex _mutex;
    bool _isFeedConnected;
    std::unordered_map<TickerId, std::string>
        _tickerIdToContract; /*!< To each data stream subscription assigned
                                  ticker id. This used to keep connection
                                  betweem contract and assigned ticker id */
};

} // namespace Ib
} // namespace Algo

#endif
