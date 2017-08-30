#include <db/Instrument.H>

namespace Algo
{
namespace Db
{
  Instrument::operator std::string() const
  {
    std::string instrument = secType()  + ":" +
                             currency() + ":" +
                             exchange() + ":" +
                             name();
    return instrument;
  }

  bool Instrument::operator==(const Instrument &rhs) const
  {
    std::string lhsStr = *this;
    std::string rhsStr = rhs;
    return (lhsStr.compare(rhsStr) == 0); 
  }

  std::ostream &operator <<(std::ostream &stream, const Instrument &rhs)
  {
    stream << (std::string) rhs;
    return stream;
  }
}
}
