#include <interactive_brokers/ContractsFactory.H>

#include <shared/easylogging++.h>
#include <ib_api/client/Contract.h>

namespace Algo
{
namespace Ib
{

std::string ContractsFactory::toString(const ::Contract& contract) {
  return contract.secType + ":" + contract.currency + ":" +
         contract.exchange + ":" + contract.localSymbol;
}

::Contract ContractsFactory::fromString(const std::string& contractStr) {
  ::Contract contract;
  std::istringstream iStrStream(contractStr);
  getline(iStrStream, contract.secType, ':');
  getline(iStrStream, contract.currency, ':');
  getline(iStrStream, contract.exchange, ':');
  getline(iStrStream, contract.localSymbol, ':');

  return contract;
}

}
}
