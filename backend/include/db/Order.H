#ifndef Order_H
#define Order_H

#include "Backend.H"
#include "EventType.H"
#include "Strategy.H"
#include "quote/Side.H"

#include <util/TimeUtil.H>
#include <shared/commands.h>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <odb/core.hxx>
#include <string>
#include <memory>

namespace Algo
{
namespace Db
{
  #pragma db object table("Orders")
  class Order
  {
    public:
      #pragma db value
      struct PrimaryKey
      {
        #pragma db column("ClientOrderId") type("VARCHAR(750)")
        std::string _clientOrderId;

        #pragma db column("BackendId")
        unsigned long _backendId;

        PrimaryKey() {}

        PrimaryKey(const std::string &clientOrderId, unsigned long backendId)
                 : _clientOrderId(clientOrderId),
                   _backendId(backendId) {}

        bool operator<(const struct PrimaryKey& rhs) const
        {
          return (_backendId < rhs._backendId && _clientOrderId.compare(rhs._clientOrderId) < 0);
        }
      };

      Order()
         : _timestamp(TimeUtil::getCurrentTime()) {}

      Order(const std::string& clientOrderId,
            const unsigned long strategyId, unsigned long backendId, 
            Algo::EventType eventType, AlgoOrderType orderType, 
            double sentQuantity, double sentPrice,
            OrderMode mode)
         : _clientOrderIdBakendId(clientOrderId, backendId),
           _strategyId(strategyId),
           _eventType(eventType),
           _orderType(orderType),
           _sentQuantity(sentQuantity),
           _sentPrice(sentPrice),
           _mode(mode),
           _timestamp(TimeUtil::getCurrentTime()) {}

      const std::string& clientOrderId() const { return _clientOrderIdBakendId._clientOrderId; }
      void setClientOrderId(const std::string& clientOrderId) { _clientOrderIdBakendId._clientOrderId = clientOrderId; }

      unsigned long backendId() const { return _clientOrderIdBakendId._backendId; }
      void setBackendId(unsigned long backendId) { _clientOrderIdBakendId._backendId = backendId; }

      unsigned long strategyId() const { return _strategyId; }
      void setStrategyId(unsigned long strategyId) { _strategyId = strategyId; }

      Algo::EventType eventType() const { return _eventType; }
      void setEventType(Algo::EventType eventType) { _eventType = eventType; }

      AlgoOrderType orderType() const { return _orderType; }
      void setOrderType(AlgoOrderType orderType) { _orderType = orderType; }

      double sentQuantity() const { return _sentQuantity; }
      void setSentQuantity(double sentQuantity)
      {
        _sentQuantity = sentQuantity;
      }

      double sentPrice() const { return _sentPrice; }
      void setSentPrice(double sentPrice) { _sentPrice = sentPrice; }

      OrderMode setMode() const { return _mode; }
      void setMode(const OrderMode mode) { _mode = mode; }

      boost::posix_time::ptime timestamp() const { return _timestamp; }
      void setTimestamp(boost::posix_time::ptime ts) { _timestamp = ts; }

    private:
      friend class odb::access;

      #pragma db column("Id")
      unsigned long _id;

      #pragma db id column("")
      struct PrimaryKey _clientOrderIdBakendId;

      #pragma db column("StrategyId") not_null
      unsigned long _strategyId;

      #pragma db column("EventType") not_null
      Algo::EventType _eventType;

      #pragma db column("OrderType") not_null
      AlgoOrderType _orderType;

      #pragma db column("SentQuantity") type("DOUBLE PRECISION(20, 10)")
      double _sentQuantity;

      #pragma db column("SentPrice") type("DOUBLE PRECISION(20, 10)")
      double _sentPrice;

      #pragma db column("Side")
      OrderMode _mode;

      // TIMESTAPMP(6) - 6 digit decimal precision of seconds
      #pragma db column("Timestamp") type("TIMESTAMP(6)") \
        options("DEFAULT CURRENT_TIMESTAMP(6)") not_null
      boost::posix_time::ptime _timestamp;
  };
} // namespace Db
} // namespace Algo

#endif // Order_H
