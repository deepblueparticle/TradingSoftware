/*
 * =====================================================================================
 *
 *       Filename:  SymbolDictionary.H
 *
 *    Description:  
 *
 *        Created:  09/17/2016 12:11:17 PM
 *       Compiler:  g++
 *
 *         Author:  Soumya Prasad Ukil
 *        Company:  AlgoEngine
 *
 * =====================================================================================
 */

#ifndef SymbolDictionary_H
#define SymbolDictionary_H

#include <string>
#include <map>
#include <symbol/SymbolStaticData.H>

namespace Algo
{
  class SymbolDictionary
  {
    public:
      static SymbolDictionary& getInstance();
      bool getSymbolData(std::string sym, SymbolStaticData sd);
      void addSymbolData(SymbolStaticData sd);
      size_t getSize();
      std::map<std::string, SymbolStaticData>& getDictonaryMap();
      int getPrecision(std::string sym);

    private:
      SymbolDictionary();
      std::map<std::string, SymbolStaticData> _dictionaryMap;
  };
}
#endif
