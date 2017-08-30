/*
 * =====================================================================================
 *
 *       Filename:  QuoteBook.C
 *
 *    Description:
 *
 *        Created:  09/21/2016 09:07:56 AM
 *       Compiler:  g++
 *
 *         Author:  Soumya Prasad Ukil
 *        Company:  AlgoEngine
 *
 * =====================================================================================
 */

#include <quote/QuoteBook.H>

#include <shared/easylogging++.h>

namespace Algo
{
  std::mutex QuoteBook::_quoteMapMutex;

  QuoteBook* QuoteBook::getInstance()
  {
    static QuoteBook qb;
    return &qb;
  }

  void QuoteBook::add(const std::string& secId, Quotes quote)
  {
    std::lock_guard<std::mutex> lock(_quoteMapMutex);
    _quoteMap[secId] = quote;
  }

  void QuoteBook::add(const std::string& securityId, double p, uint32_t q, Side s)
  {
    std::lock_guard<std::mutex> lock(_quoteMapMutex);
    time_t now;
    time(&now);
    if(s == Side_BID)
    {
      _quoteMap[securityId].setBidPrice(p);
      _quoteMap[securityId].setBidQty(q);
      _quoteMap[securityId].setTimeStamp(now);
    }
    else
    {
      _quoteMap[securityId].setAskPrice(p);
      _quoteMap[securityId].setAskQty(p);
      _quoteMap[securityId].setTimeStamp(now);
    }
  }

  void QuoteBook::del(const std::string& securityId)
  {
    std::lock_guard<std::mutex> lock (_quoteMapMutex);
    _quoteMap.erase(securityId);
  }

  Quotes QuoteBook::get(const std::string& secId)
  {
    std::lock_guard<std::mutex> lock(_quoteMapMutex);
    return _quoteMap[secId];
  }

  void QuoteBook::clear()
  {
    std::lock_guard<std::mutex> lock (_quoteMapMutex);
    _quoteMap.clear();
  }

  void QuoteBook::dump(const std::string& secId)
  {
    std::lock_guard<std::mutex> lock(_quoteMapMutex);
    _quoteMap[secId].dump();
  }

  QuoteBook::QuoteBook()
  {
  }

  void QuoteBook::modify(const std::string& securityId, double p, u_int32_t q, Side s )
  {
    std::lock_guard<std::mutex> lock(_quoteMapMutex);
    time_t now;
    time(&now);
    if(s == Side_BID)
    {
      _quoteMap[securityId].setBidPrice(p);
      _quoteMap[securityId].setBidQty(q);
      _quoteMap[securityId].setTimeStamp(now);
    }
    else
    {
      _quoteMap[securityId].setAskPrice(p);
      _quoteMap[securityId].setAskQty(p);
      _quoteMap[securityId].setTimeStamp(now);
    }
  }
}
