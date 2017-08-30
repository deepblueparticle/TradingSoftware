#ifndef RejectEvent_H
#define RejectEvent_H

#include "Order.H"

#include <boost/date_time/posix_time/posix_time.hpp>

#include <odb/core.hxx>
#include <string>

namespace Algo
{
namespace Db
{
  #pragma db object table("RejectEvents")
  class RejectEvent
  {
    public:
      RejectEvent() {}

      RejectEvent(const std::string clientOrderId, const unsigned long backendId, const std::string reason)
         : _clientOrderId(clientOrderId),
           _backendId(backendId),
           _reason(reason) {}

      const std::string clientOrderId() const { return _clientOrderId; }
      void setClientOrderId(const std::string& clientOrderId)
      {
        _clientOrderId = clientOrderId;
      }

      unsigned long backendId() const { return _backendId; }
      void setBackendId(unsigned long backendId) 
      { 
        _backendId = backendId; 
      }

      const std::string& reason() const { return _reason; }
      void setReason(const std::string& reason) { _reason = reason; }

      boost::posix_time::ptime timestamp() const { return _timestamp; }
      void setTimestamp(boost::posix_time::ptime ts) { _timestamp = ts; }

     private:
      friend class odb::access;

      #pragma db id column("Id")
      unsigned long _id;

      #pragma db column("ClientOrderId") not_null
      std::string _clientOrderId;

      #pragma db column("BackendId")
      unsigned long _backendId;

      #pragma db column("Reason") type("VARCHAR(2048)")
      std::string _reason;

      // TIMESTAPMP(6) - 6 digit decimal precision of seconds
      #pragma db column("Timestamp") type("TIMESTAMP(6)") \
        options("DEFAULT CURRENT_TIMESTAMP(6)") not_null
      boost::posix_time::ptime _timestamp;
  };
} // namespace Db
} // namespace Algo

#endif // RejectEvent_H
