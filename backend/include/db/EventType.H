#ifndef EventType_H
#define EventType_H

#include <shared/EventType.H>

#include <odb/core.hxx>
#include <string>

namespace Algo
{
namespace Db
{
  #pragma db object table("EventTypes")
  class EventType
  {
    public:
      EventType(const std::string& name): _name(name) {}

      const std::string& name() const { return _name; }
      void setName(const std::string& name) { _name = name; }

    private:
      EventType() {}

      friend class odb::access;

      #pragma db id column("Id")
      Algo::EventType _id;

      #pragma db column("Name")
      std::string _name;
  };
} // namespace Db
} // namespace Algo

#endif // EventType_H
