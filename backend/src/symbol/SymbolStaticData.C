/*
 * =====================================================================================
 *
 *       Filename:  SymbolStaticData.C
 *
 *    Description:  
 *
 *        Created:  09/22/2016 01:06:42 PM
 *       Compiler:  g++
 *
 *         Author:  Soumya Prasad Ukil
 *        Company:  AlgoEngine
 *
 * =====================================================================================
 */

#include <sstream>
#include <symbol/SymbolStaticData.H>

namespace Algo
{
  std::string SymbolStaticData::getHeader()
  {
    std::string header;
    header = "Symbol Name,ProductType,Factor,Contract Multiplier,";
    header += "Currency,LotSize,FXCNsymid,FXCMSym Precision,";
    header += "FXCMSymPointSize,FXCMMaxQuantity,FXCMMinQuantity";
    return header;
  }

  std::string SymbolStaticData::toString()
  {
    std::stringstream ss;
    ss << _symbolName << ',' 
      << _productType << ','
      << _currency << ',' 
      << _lotSize << ',' 
      << _FXCNsymid << ','
      << _FXCMSymPrecision << ',' 
      << _FXCMSymPointSize << ','
      << _FXCMMaxQuantity <<',' 
      << _FXCMMinQuantity <<'\n';
    return ss.str();
  }

  std::string SymbolStaticData::getSymbolName()
  {
    return _symbolName;
  }

  int SymbolStaticData::getFXCMSymPrecision()
  {
    return _FXCMSymPrecision;
  }

  int SymbolStaticData::getFXCMSymPointSize()
  {
    return _FXCMSymPointSize;
  }

  int SymbolStaticData::getLotSize()
  {
    return _lotSize;
  }

  void SymbolStaticData::setSymbolName(std::string symbolName_)
  {
    _symbolName = symbolName_;
  }

  void SymbolStaticData::setProductType(ProductType productType_)
  {
    _productType = productType_;
  }

  void SymbolStaticData::setFactor(float factor_)
  {
    _factor = factor_;
  }

  void SymbolStaticData::setContractMultiplier(float contractMultiplier_)
  {
    _contractMultiplier = contractMultiplier_;
  }

  void SymbolStaticData::setCurrency(std::string currency_)
  {
    _currency = currency_;
  }

  void SymbolStaticData::setLotSize(uint32_t lotSize_)
  {
    _lotSize = lotSize_;
  }

  void SymbolStaticData::setFXCMSymId(std::string FXCNsymid_)
  {
    _FXCNsymid = FXCNsymid_;
  }

  void SymbolStaticData::setFXCMSymPrecision(uint8_t FXCMSymPrecision_)
  {
    _FXCMSymPrecision = FXCMSymPrecision_;
  }

  void SymbolStaticData::setFXCMSymPointSize(float FXCMSymPointSize_)
  {
    _FXCMSymPointSize = FXCMSymPointSize_;
  }
}
