/*
 * =====================================================================================
 *
 *       Filename:  SymbolDictionary.C
 *
 *    Description:  
 *
 *        Created:  09/22/2016 12:45:36 PM
 *       Compiler:  g++
 *
 *         Author:  Soumya Prasad Ukil
 *        Company:  AlgoEngine
 *
 * =====================================================================================
 */

#include <shared/easylogging++.h>
#include <symbol/SymbolDictionary.H>

namespace Algo
{
  SymbolDictionary& SymbolDictionary::getInstance()
  {
    static SymbolDictionary symbolDictionary;
    return symbolDictionary;
  }

  bool SymbolDictionary::getSymbolData(std::string sym, SymbolStaticData sd)
  {
    auto itr = _dictionaryMap.find(sym);
    if( itr == _dictionaryMap.end())
      return false;
    sd = itr->second;
    return true;
  }

  void SymbolDictionary::addSymbolData(SymbolStaticData sd)
  {
    LOG(INFO) << "[Other] Adding "<< sd.getSymbolName() << " in dictionary.";
    _dictionaryMap[sd.getSymbolName()] = sd;
  }

  size_t SymbolDictionary::getSize()
  {
    return _dictionaryMap.size();
  }

  std::map<std::string, SymbolStaticData>& SymbolDictionary::getDictonaryMap()
  {
    return _dictionaryMap;
  }

  int SymbolDictionary::getPrecision(std::string sym)
  {
    auto itr = _dictionaryMap.find(sym);
    if( itr == _dictionaryMap.end())
      return 5;
    return itr->second.getFXCMSymPrecision();
  }

  SymbolDictionary::SymbolDictionary()
  {
  }
}
