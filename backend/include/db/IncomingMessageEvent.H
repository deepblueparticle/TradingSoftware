#ifndef IncomingMessageEvent_H
#define IncomingMessageEvent_H

#include "EventType.H"
#include "Order.H"

#include <shared/commands.h>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <odb/core.hxx>
#include <string>

namespace Algo
{
namespace Db
{
  #pragma db object table("IncomingMessageEvents")
  class IncomingMessageEvent
  {
    public:
      IncomingMessageEvent() {}

      IncomingMessageEvent(const std::string& clientOrderId,
                           const unsigned long backendId,
                           const std::string exchangeOrderId,
                           Algo::EventType eventType, double receivedQuantity,
                           double receivedPrice)
         : _clientOrderId(clientOrderId),
           _backendId(backendId),
           _exchangeOrderId(exchangeOrderId),
           _eventType(eventType),
           _receivedQuantity(receivedQuantity),
           _receivedPrice(receivedPrice) {}

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

      const std::string& exchangeOrderId() const { return _exchangeOrderId; }
      void setExchangeOrderId(const std::string& exchangeOrderId)
      {
        _exchangeOrderId = exchangeOrderId;
      }

      Algo::EventType eventType() const { return _eventType; }
      void setEventType(Algo::EventType eventType) { _eventType = eventType; }

      double receivedQuantity() const { return _receivedQuantity; }
      void setReceivedQuantity(double receivedQuantity)
      {
       _receivedQuantity = receivedQuantity;
      }

      double receivedPrice() const { return _receivedPrice; }
      void setReceivedPrice(double receivedPrice)
      {
        _receivedPrice = receivedPrice;
      }

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

      #pragma db column("ExchangeOrderId") type("VARCHAR(1024)")
      std::string _exchangeOrderId;

      #pragma db column("EventType") not_null
      Algo::EventType _eventType;

      #pragma db column("ReceivedQuantity") type("DOUBLE PRECISION(20, 10)")
      double _receivedQuantity;

      #pragma db column("ReceivedPrice") type("DOUBLE PRECISION(20, 10)")
      double _receivedPrice;

      // TIMESTAPMP(6) - 6 digit decimal precision of seconds
      #pragma db column("Timestamp") type("TIMESTAMP(6)") \
        options("DEFAULT CURRENT_TIMESTAMP(6)") not_null
      boost::posix_time::ptime _timestamp;
  };
} // namespace Db
} // namespace Algo

#endif // IncomingMessageEvent_H
