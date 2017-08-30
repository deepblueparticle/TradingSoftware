#ifndef Backend_H
#define Backend_H

#include <odb/core.hxx>
#include <string>

namespace Algo
{
namespace Db
{
  #pragma db object table("Backends")
  class Backend
  {
    public:
      static Backend& getInstance();

      unsigned long id() const { return _id; }

      const std::string& uuid() const { return _uuid; }
      void setUuid(const std::string& uuid) { _uuid = uuid; }

      const std::string& buyAccount() const { return _buyAccount; }
      void setBuyAccount(const std::string& buyAccount)
      {
        _buyAccount = buyAccount;
      }

      const std::string& sellAccount() const { return _sellAccount; }
      void setSellAccount(const std::string& sellAccount)
      {
        _sellAccount = sellAccount;
      }

      const std::string& dataFeedVenueId() const { return _dataFeedVenueId; }
      void setDataFeedVenueId(const std::string& dataFeedVenueId) { _dataFeedVenueId = dataFeedVenueId; }

      const std::string& orderVenueId() const { return _orderVenueId; }
      void setOrderVenueId(const std::string& orderVenueId) { _orderVenueId = orderVenueId; }

      std::string getValues();

    private:
      Backend(const std::string& uuid, const std::string& buyAccount,
              const std::string& sellAccount, const std::string& dataFeedVenueId,
              const std::string& orderVenueId)
         : _uuid(uuid),
           _buyAccount(buyAccount),
           _sellAccount(sellAccount),
           _dataFeedVenueId(dataFeedVenueId),
           _orderVenueId(orderVenueId) {}

      Backend() {}

      friend class odb::access;

      #pragma db id auto column("Id")
      unsigned long _id;

      #pragma db column("Uuid") type("VARCHAR(36)")
      std::string _uuid;

      #pragma db column("BuyAccount") type("VARCHAR(1024)")
      std::string _buyAccount;

      #pragma db column("SellAccount") type("VARCHAR(1024)")
      std::string _sellAccount;

      #pragma db column("DataFeedVenueId") type("VARCHAR(1024)")
      std::string _dataFeedVenueId;

      #pragma db column("OrderVenueId") type("VARCHAR(1024)")
      std::string _orderVenueId;
  };
} // namespace Db
} // namespace Algo

#endif // Backend_H
