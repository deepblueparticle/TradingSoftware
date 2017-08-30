#ifndef Instrument_H
#define Instrument_H

#include <odb/core.hxx>
#include <string>
#include <boost/serialization/access.hpp>
#include <boost/serialization/map.hpp>

namespace Algo
{
namespace Db
{
  #pragma db object table("Instruments")
  class Instrument
  {
    public:
      friend class boost::serialization::access;
      Instrument() {}
      Instrument(const std::string& name,     const std::string& secType,
                 const std::string& exchange, const std::string& currency,
                 const std::string& type) : _name(name), _secType(secType),
                                            _exchange(exchange), _currency(currency),
                                            _type(type) {}

      unsigned long id() const { return _id; }
      void setId(unsigned long id) { _id = id; }

      const std::string& name() const { return _name; }
      void setName(const std::string& name) { _name = name; }

      const std::string& secType() const { return _secType; }
      void setSecType(const std::string& secType) { _secType = secType; }

      const std::string& exchange() const { return _exchange; }
      void setExchange(const std::string& exchange) { _exchange = exchange; }

      const std::string& currency() const { return _currency; }
      void setCurrency(const std::string& currency) { _currency = currency; }

      const std::string& type() const { return _type; }
      void setType(const std::string& type) { _type = type; }

      bool operator==(const Instrument &rhs) const;
      operator std::string() const;

    private:
      friend class odb::access;

      #pragma db id auto column("Id")
      unsigned long _id;

      #pragma db column("Name")
      std::string _name;

      #pragma db column("SecType")
      std::string _secType;

      #pragma db column("Exchange")
      std::string _exchange;

      #pragma db column("Currency")
      std::string _currency;

      #pragma db column("Type")
      std::string _type;

      template<class Archive>
      void serialize(Archive &ar, const unsigned int version)
      {
        ar & _id & _name & _secType & _exchange & _currency & _type;
      }
  };
  std::ostream &operator <<(std::ostream &stream, const Instrument &rhs);
} // namespace Db
} // namespace Algo

#endif // Instrument_H
