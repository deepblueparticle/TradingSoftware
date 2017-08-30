#ifndef IMARKET_DATA_HANDLER_H
#define IMARKET_DATA_HANDLER_H

#include <string>
#include <unordered_map>
#include <shared/easylogging++.h>

namespace Algo {

/*! \class    IMarketDataHandler
 *  \brief    Interface class handling market data subscription
 *
 */
class IMarketDataHandler
{
  public:
    virtual ~IMarketDataHandler() {}

    /*!
     * \brief Subscribe to market data stream for symbol (instrument).
     *        Internally keeps counter of number of subscriptions per symbol.
     *        And hence subscribes only once per symbol.
     *
     * \param symbol Name of the symbol.
     * \return True if subscribed, otherwise false.
     */
    bool subscribe(const std::string& symbol) {
      _subscriptionCounter[symbol]++;
      // subscribe only once if subscription counter moved from 0 to 1
      if (_subscriptionCounter[symbol] == 1) {
        bool result = subscribeImplementation(symbol);
        if (result == false) {
          _subscriptionCounter[symbol]--;
        }
        return result;
      }
      return true;
    }

    /*!
     * \brief It checks if connection is still alive or not
     */
    virtual bool isConnected() = 0;

    /*!
     * \brief Unsubscribe from market data stream for symbol (instrument).
     *        Internally keeps counter of number of subscriptions per symbol.
     *        And hence unsubscribes only once when counter hits 0.
     *
     * \param symbol Name of the symbol.
     * \return True if unsubscribed, otherwise false.
     */
    bool unsubscribe(const std::string& symbol) {
      if (_subscriptionCounter[symbol] > 0) {
        _subscriptionCounter[symbol]--;

        if (_subscriptionCounter[symbol] <= 0) {
          bool status = unsubscribeImplementation(symbol);
          if (status == true) {
            _subscriptionCounter.erase(symbol);
          }
          return status;
        }
      }
      else
      {
        LOG(ERROR) << "[IB MD] Unsubscription fails, not subscribed yet!";
      }
      return true;
    }

  protected:

    /*!
     * \brief Real implementation of data subscription to market data. Should
     *        be implemented by classes that implement IMarketDataHandler.
     *
     * \param symbol Name of the symbol.
     * \return True if subscribed, otherwise false.
     */
    virtual bool subscribeImplementation(const std::string& symbol) = 0;

    /*!
     * \brief Real implementation of unsubscription from market data. Should
     *        be implemented by classes that implement IMarketDataHandler.
     *
     * \param symbol Name of the symbol.
     * \return True if unsubscribed, otherwise false.
     */
    virtual bool unsubscribeImplementation(const std::string& symbol) = 0;

    std::unordered_map<std::string, int> _subscriptionCounter;
};

}

#endif // IMARKET_DATA_HANDLER_H
