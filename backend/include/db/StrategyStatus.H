#ifndef StrategyStatus_H
#define StrategyStatus_H

#include <constant/AlgoStrategyStatus.H>

#include <odb/core.hxx>
#include <string>

namespace Algo
{
namespace Db
{
  #pragma db object table("StrategyStatuses")
  class StrategyStatus
  {
    public:
      StrategyStatus(const std::string& name): _name(name) {}

      const std::string& name() const { return _name; }
      void setName(const std::string& name) { _name = name; }

    private:
      StrategyStatus() {}

      friend class odb::access;

      #pragma db id column("Id")
      AlgoStrategyStatus _id;

      #pragma db column("Name")
      std::string _name;
  };
} // namespace Db
} // namespace Algo

#endif // StrategyStatus_H
