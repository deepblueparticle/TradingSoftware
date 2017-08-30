#ifndef InternalEvent_H
#define InternalEvent_H

#include "EventType.H"
#include "Strategy.H"

#include <util/TimeUtil.H>
#include <shared/commands.h>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <odb/core.hxx>
#include <string>

namespace Algo
{
namespace Db
{
  #pragma db object table("InternalEvents")
  class InternalEvent
  {
    public:
      InternalEvent()
         : _timestamp(TimeUtil::getCurrentTime()) {}

      InternalEvent(const std::shared_ptr<Strategy>& strategy,
                    Algo::EventType eventType, double eventPrice)
         : _strategy(strategy),
           _eventType(eventType),
           _eventPrice(eventPrice),
           _timestamp(TimeUtil::getCurrentTime()) {}

      const std::shared_ptr<Strategy> strategy() const { return _strategy; }
      void setStrategy(const std::shared_ptr<Strategy>& strategy)
      {
        _strategy = strategy;
      }

      Algo::EventType eventType() const { return _eventType; }
      void setEventType(Algo::EventType eventType) { _eventType = eventType; }

      double eventPrice() const { return _eventPrice; }
      void setEventPrice(double eventPrice) { _eventPrice = eventPrice; }

      boost::posix_time::ptime timestamp() const { return _timestamp; }
      void setTimestamp(boost::posix_time::ptime ts) { _timestamp = ts; }

    private:
      friend class odb::access;

      #pragma db id column("Id")
      unsigned long _id;

      #pragma db column("") not_null
      std::shared_ptr<Strategy> _strategy;

      #pragma db column("EventType") not_null
      Algo::EventType _eventType;

      #pragma db column("EventPrice") type("DOUBLE PRECISION(20, 10)")
      double _eventPrice;

      // TIMESTAPMP(6) - 6 digit decimal precision of seconds
      #pragma db column("Timestamp") type("TIMESTAMP(6)") \
        options("DEFAULT CURRENT_TIMESTAMP(6)") not_null
      boost::posix_time::ptime _timestamp;
  };
} // namespace Db
} // namespace Algo

#endif // InternalEvent_H
