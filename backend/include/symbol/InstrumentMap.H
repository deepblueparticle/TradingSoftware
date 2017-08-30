/*
 * =====================================================================================
 *
 *       Filename:  InstrumentMap.H
 *
 *    Description:
 *
 *        Created:  04/01/2017 12:55:41 PM
 *       Compiler:  g++
 *
 *         Author:  Soumya Prasad Ukil
 *        Company:  AlgoEngine
 *
 * =====================================================================================
 */

#ifndef InstrumentMap_H
#define InstrumentMap_H

#include <map>
#include <string>

namespace Algo
{
  class InstrumentMap
  {
  public:
    static InstrumentMap& getInstance();
    int getSymbolId(const std::string&);
    void pushSymbol(const int&, const std::string&);

  private:
    std::map<std::string, int> _instrumentMap;
  };
}

#endif
