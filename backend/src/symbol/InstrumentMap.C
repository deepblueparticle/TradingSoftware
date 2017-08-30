/*
 * =====================================================================================
 *
 *       Filename:  InstrumentMap.C
 *
 *    Description:
 *
 *        Created:  04/01/2017 12:45:36 PM
 *       Compiler:  g++
 *
 *         Author:  Soumya Prasad Ukil
 *        Company:  AlgoEngine
 *
 * =====================================================================================
 */

#include <symbol/InstrumentMap.H>

namespace Algo
{
  InstrumentMap& InstrumentMap::getInstance()
  {
    static InstrumentMap instrumentMap;
    return instrumentMap;
  }

  int InstrumentMap::getSymbolId(const std::string &symbolName_)
  {
     if (_instrumentMap.find(symbolName_) != _instrumentMap.end())
       return _instrumentMap[symbolName_];
     return -1;
  }

  void InstrumentMap::pushSymbol(const int &symbolId_, const std::string &symbolName_)
  {
     _instrumentMap[symbolName_] = symbolId_;
  }
}
