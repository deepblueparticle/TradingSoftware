#include <db/Backend.H>

namespace Algo
{
namespace Db
{
  Backend& Backend::getInstance()
  {
    static Backend backend;
    return backend;
  }

  std::string Backend::getValues()
  {
    std::string output = "";

    output = "Id=" + std::to_string(_id) + ";";
    output += "Uuid=" + _uuid + ";";
    output += "BuyAccount=" + _buyAccount + ";";
    output += "SellAccount=" + _sellAccount + ";";
    output += "DataFeedVenueId=" + _dataFeedVenueId + ";";
    output += "OrderVenueId=" + _orderVenueId;
    return output;
  }
}
}
