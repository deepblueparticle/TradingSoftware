/*
 * =====================================================================================
 *
 *       Filename:  QuoteBook.H
 *
 *    Description:
 *
 *        Created:  09/18/2016 03:58:35 AM
 *       Compiler:  g++
 *
 *         Author:  Soumya Prasad Ukil
 *        Company:  AlgoEngine
 *
 * =====================================================================================
 */

#ifndef QuoteBook_H
#define QuoteBook_H

#include <quote/Side.H>
#include <shared/iquotesprovider.h>
#include <mutex>
#include <unordered_map>

namespace Algo
{
  class QuoteBook : public IQuotesProvider
  {
    public:
      static QuoteBook* getInstance();
      void add(const std::string& secId, Quotes quote);
      void add(const std::string& secId, double price, uint32_t quantity,
               Side side);
      void modify(const std::string& secId, double price, uint32_t quantity,
                  Side side);
      void del(const std::string& secId);
      void clear();
      Quotes get(const std::string& instrument) override;
      void dump(const std::string& secId);

    private:
      QuoteBook();
      QuoteBook(const QuoteBook&);
      std::unordered_map <std::string, Quotes> _quoteMap;
      static std::mutex                        _quoteMapMutex;
  };
}

#endif
