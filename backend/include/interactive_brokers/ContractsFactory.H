#ifndef ContractsFactory_H
#define ContractsFactory_H

#include <interactive_brokers/ConnectionHandler.H>

namespace Algo
{
namespace Ib
{

/*! \class    ContractsFactory
  * \brief    Factory class to create IB API Contract class instances
  */
class ContractsFactory
{
  public:
    /*!
     * \brief Serialize IB Contract to string.
     *
     * \param contract IB API contract.
     * \return String representation of contract
     *
     * \see ContractsFactory::fromString()
     */
    static std::string toString(const ::Contract& contract);

    /*!
     * \brief Serialize IB Contract to string
     *
     * \param contractStr String representation of contract
     *                    (\see ContractsFactory::toString())
     * \return IB API Contract
     */
    static ::Contract fromString(const std::string& contractStr);

  protected:
    ContractsFactory() {}
};

} // namespace Ib
} // namespace Algo

#endif
