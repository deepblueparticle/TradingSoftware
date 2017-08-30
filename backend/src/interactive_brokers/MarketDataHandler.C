#include <algorithm>
#include <iterator>

#include <interactive_brokers/MarketDataHandler.H>

#include <shared/easylogging++.h>

#include <ib_api/client/EClientSocket.h>
#include <ib_api/client/EReader.h>
#include <quote/QuoteBook.H>

#ifdef IB_PAPER_ACCOUNT
# define ALGO_IB_BID DELAYED_BID
# define ALGO_IB_ASK DELAYED_ASK
#else
# define ALGO_IB_BID BID
# define ALGO_IB_ASK ASK
#endif

namespace Algo
{
namespace Ib
{

MarketDataHandler::MarketDataHandler() : _isFeedConnected(true)
{}

MarketDataHandler::~MarketDataHandler() {}

bool MarketDataHandler::subscribeImplementation(const std::string& symbol) {
  static int sTickerId = 1;

  if (this->isConnected()) 
  {
    #ifdef IB_PAPER_ACCOUNT
    // Switch to live (1) frozen (2) delayed (3) or delayed frozen (4)
    _client->reqMarketDataType(3);
    #else
    _client->reqMarketDataType(1);
    #endif

    ::Contract ibContract = ContractsFactory::fromString(symbol);
    LOG(INFO) << ibContract.secType << " " << ibContract.currency 
              << " " << ibContract.exchange << " " << ibContract.localSymbol;

    _client->reqMktData(sTickerId, ibContract, "", false, TagValueListSPtr());
    std::lock_guard<std::mutex> guard(_mutex);
    _tickerIdToContract[sTickerId] = symbol;

    LOG(INFO) << "[IB MD] Subscribed to " << symbol << " tickerId " << sTickerId;
    ++sTickerId;

    return true;
  }
  else 
  {
    LOG(INFO) << "[IB MD] Not Subscribed to " << symbol << " since Feed connection is not yet ready.";
  }
}

bool MarketDataHandler::isConnected()
{
  return ConnectionHandler::isConnected() && _isFeedConnected;
}

bool MarketDataHandler::connect(const std::string& host, const int port)
{
  bool status = ConnectionHandler::connect(host, port);
  if (status) 
  {
    reSubscribeSymbols();
  }
  return status;
}

void MarketDataHandler::reSubscribeSymbols()
{
  for (const auto &pair : _subscriptionCounter)
  {
    subscribeImplementation(pair.first);
  }
}

bool MarketDataHandler::unsubscribeImplementation(const std::string& symbol) {
  if (this->isConnected())
  {
    std::lock_guard<std::mutex> guard(_mutex);
    auto tickerContract =
        std::find_if(_tickerIdToContract.begin(), _tickerIdToContract.end(),
                     [=](const std::pair<TickerId, std::string>& pair) {
                       return pair.second == symbol;
                     });
    if (tickerContract == _tickerIdToContract.end()) {
      LOG(ERROR) << "[IB MD] Trying to unsubscribe from " << symbol
                 << " while it is not subscribed to it.";
      return false;
    }

    int tickerId = tickerContract->first;
    _client->cancelMktData(tickerId);
    _tickerIdToContract.erase(tickerContract);
    QuoteBook::getInstance()->del(symbol);

    LOG(INFO) << "[IB MD] Unsubscribed from " << symbol << " tickerId " << tickerId;

    return true;
  }
  else
  {
    LOG(INFO) << "[IB MD] Not Unsubscribed to " << symbol << " since Feed connection is not yet ready.";
  }
}

void MarketDataHandler::tickPrice(TickerId tickerId, TickType field,
                                  double price, int canAutoExecute) {
  if (this->isConnected()) {
    std::string symbol = _tickerIdToContract[tickerId];

    switch (field) {
      case ALGO_IB_BID:
        if (price < 0) {
          LOG(TRACE) << "[IB MD] Ticker " << symbol
                     << " receiving negative ticker price " << price << " ignoring.";
          break;
        }
        LOG(INFO) << "[IB MD] Ticker " << symbol << " BID price: " << price;
        QuoteBook::getInstance()->modify(symbol, price, 0, Side_BID);
        break;

      case ALGO_IB_ASK:
        if (price < 0) {
          LOG(TRACE) << "[IB MD] Ticker " << symbol
                     << " receiving negative ticker price " << price << " ignoring.";
          break;
        }
        LOG(INFO) << "[IB MD] Ticker " << symbol << " ASK price: " << price;
        QuoteBook::getInstance()->modify(symbol, price, 0, Side_ASK);
        break;

      default:
        LOG(TRACE) << "[IB MD] Ticker " << symbol
                   << " skipping recieved ticker type " << field;
    }
  }
  else {
    LOG(TRACE) << "[IB MD] Ignoring prices since connection down.";
  }
}

void MarketDataHandler::error(const int id, const int errorCode,
                              const std::string errorStr) {
  LOG(ERROR) << "[IB MD] Id: " << id << " Code: " << errorCode << " Msg: " << errorStr;
  switch (errorCode) {
    case 1100:
      QuoteBook::getInstance()->clear();
      _isFeedConnected = false;
      break;
    case 1101:
    case 1102:
      reSubscribeSymbols();
      _isFeedConnected = true;
      break;
    case 200:
      {
        std::lock_guard<std::mutex> guard(_mutex);
        auto iter = _tickerIdToContract.find(id);
        if (iter != _tickerIdToContract.end())
        {
          _tickerIdToContract.erase(iter);
          QuoteBook::getInstance()->del(iter->second);
        }
      }
   }
}
}
}
