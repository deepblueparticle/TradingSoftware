#ifndef OrderType_H
#define OrderType_H

#include <odb/core.hxx>
#include <string>
#include <constant/AlgoOrderType.H>

namespace Algo
{
namespace Db
{
  #pragma db object table("OrderTypes")
  class OrderType
  {
    public:
      OrderType(const std::string& name): _name(name) {}

      const std::string& name() const { return _name; }
      void setName(const std::string& name) { _name = name; }

    private:
      OrderType() {}

      friend class odb::access;

      #pragma db id column("Id")
      AlgoOrderType _id;

      #pragma db column("Name")
      std::string _name;
  };
} // namespace Db
} // namespace Algo

#endif // OrderType_H
