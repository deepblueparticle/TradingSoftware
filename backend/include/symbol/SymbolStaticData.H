/*
 * =====================================================================================
 *
 *       Filename:  SymbolStaticData.H
 *
 *    Description:  
 *
 *        Created:  09/17/2016 09:50:17 AM
 *       Compiler:  g++
 *
 *         Author:  Soumya Prasad Ukil
 *        Company:  AlgoEngine
 *
 * =====================================================================================
 */

#include <string>
#include <stdint.h>
#include <symbol/ProductType.H>

namespace Algo
{
  class SymbolStaticData
  {
    public:
      std::string getHeader();
      std::string toString();
      std::string getSymbolName();
      int getFXCMSymPrecision();
      int getFXCMSymPointSize();
      int getLotSize();
      void setSymbolName(std::string);
      void setProductType(ProductType);
      void setFactor(float);
      void setContractMultiplier(float);
      void setCurrency(std::string);
      void setLotSize(uint32_t);
      void setFXCMSymId(std::string);
      void setFXCMSymPrecision(uint8_t);
      void setFXCMSymPointSize(float);

    private:
      std::string  _symbolName;
      ProductType  _productType;
      float        _factor;
      float        _contractMultiplier;
      std::string  _currency;
      uint32_t     _lotSize;
      std::string  _FXCNsymid;
      uint8_t      _FXCMSymPrecision;
      float        _FXCMSymPointSize;
      uint32_t     _FXCMMaxQuantity;
      uint32_t     _FXCMMinQuantity;
  };
}
