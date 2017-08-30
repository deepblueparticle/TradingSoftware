#include <db/Strategy.H>

namespace Algo
{
namespace Db
{
  Strategy::Strategy() {}

  unsigned long Strategy::strategyId() const
  {
    return _strategyIdBakendId._strategyId;
  }

  void Strategy::setStrategyId(unsigned long strategyId)
  {
    _strategyIdBakendId._strategyId = strategyId;
  }

  unsigned long Strategy::backendId() const
  {
    return _strategyIdBakendId._backendId;
  }

  void Strategy::setBackendId(unsigned long backendId)
  {
    _strategyIdBakendId._backendId = backendId;
  }

  int Strategy::internalId() const
  {
    return _internalId;
  }

  void Strategy::setInternalId(int internalId)
  {
    _internalId = internalId;
  }

  std::string Strategy::uploadId() const
  {
    return _uploadId;
  }

  void Strategy::setUploadId(const std::string &uploadId)
  {
    _uploadId = uploadId;
  }

  unsigned long Strategy::instrumentId() const
  {
    return _instrumentId;
  }

  void Strategy::setInstrumentId(unsigned long instrumentId)
  {
    _instrumentId = instrumentId;
  }

  double Strategy::tickSize() const
  {
    return _tickSize;
  }

  void Strategy::setTickSize(double tickSize)
  {
    _tickSize = tickSize;
  }

  AlgoType Strategy::algoType() const
  {
    return _algoType;
  }

  void Strategy::setAlgoType(const AlgoType algoType)
  {
    _algoType = algoType;
  }

  OrderMode Strategy::mode() const
  {
    return _mode;
  }

  void Strategy::setMode(const OrderMode mode)
  {
    _mode = mode;
  }

  double Strategy::previousClose() const
  {
    return _previousClose;
  }

  void Strategy::setPreviousClose(double previousClose)
  {
    _previousClose = previousClose;
  }

  double Strategy::baseValue() const
  {
    return _baseValue;
  }

  void Strategy::setBaseValue(double baseValue)
  {
    _baseValue = baseValue;
  }

  double Strategy::pointAPrice() const
  {
    return _pointAPrice;
  }

  void Strategy::setPointAPrice(double pointAPrice)
  {
    _pointAPrice = pointAPrice;
  }

  double Strategy::pointAPercentage() const
  {
    return _pointAPercentage;
  }

  void Strategy::setPointAPercentage(double pointAPercentage)
  {
    _pointAPercentage = pointAPercentage;
  }

  double Strategy::pointBPrice() const
  {
    return _pointBPrice;
  }

  void Strategy::setPointBPrice(double pointBPrice)
  {
    _pointBPrice = pointBPrice;
  }

  double Strategy::pointBPercentage() const
  {
    return _pointBPercentage;
  }

  void Strategy::setPointBPercentage(double pointBPercentage)
  {
    _pointBPercentage = pointBPercentage;
  }

  double Strategy::pointCPrice() const
  {
    return _pointCPrice;
  }

  void Strategy::setPointCPrice(double pointCPrice)
  {
    _pointCPrice = pointCPrice;
  }

  double Strategy::pointCPercentage() const
  {
    return _pointCPercentage;
  }

  void Strategy::setPointCPercentage(double pointCPercentage)
  {
    _pointCPercentage = pointCPercentage;
  }

  double Strategy::CP1Price() const
  {
    return _CP1Price;
  }

  void Strategy::setCP1Price(double CP1Price)
  {
    _CP1Price = CP1Price;
  }

  double Strategy::CP1Percentage() const
  {
    return _CP1Percentage;
  }

  void Strategy::setCP1Percentage(double CP1Percentage)
  {
    _CP1Percentage = CP1Percentage;
  }

  double Strategy::CP2Price() const
  {
    return _CP2Price;
  }

  void Strategy::setCP2Price(double CP2Price)
  {
    _CP2Price = CP2Price;
  }

  double Strategy::CP2Percentage() const
  {
    return _CP2Percentage;
  }

  void Strategy::setCP2Percentage(double CP2Percentage)
  {
    _CP2Percentage = CP2Percentage;
  }

  double Strategy::ParentOrderPrice() const
  {
    return _parentOrderPrice;
  }

  void Strategy::setParentOrderPrice(double parentOrderPrice)
  {
    _parentOrderPrice = parentOrderPrice;
  }

  double Strategy::profit() const
  {
    return _profit;
  }

  void Strategy::setProfit(double profit)
  {
    _profit = profit;
  }

  double Strategy::profitPercentage() const
  {
    return _profitPercentage;
  }

  void Strategy::setProfitPercentage(double profitPercentage)
  {
    _profitPercentage = profitPercentage;
  }

  double Strategy::loss() const
  {
    return _loss;
  }

  void Strategy::setLoss(double loss)
  {
    _loss = loss;
  }

  double Strategy::lossPercentage() const
  {
    return _lossPercentage;
  }

  void Strategy::setLossPercentage(double lossPercentage)
  {
    _lossPercentage = lossPercentage;
  }

  double Strategy::lossLimitPrice() const
  {
    return _lossLimitPrice;
  }

  void Strategy::setLossLimitPrice(double lossLimitPrice)
  {
    _lossLimitPrice = lossLimitPrice;
  }

  double Strategy::trailing() const
  {
    return _trailing;
  }

  void Strategy::setTrailing(double trailing)
  {
    _trailing = trailing;
  }

  double Strategy::trailingPercentage() const
  {
    return _trailingPercentage;
  }

  void Strategy::setTrailingPercentage(double trailingPercentage)
  {
    _trailingPercentage = trailingPercentage;
  }

  double Strategy::tolerance() const
  {
    return _tolerance;
  }

  void Strategy::setTolerance(double tolerance)
  {
    _tolerance = tolerance;
  }

  boost::gregorian::date Strategy::startDate() const
  {
    return _startDate;
  }

  void Strategy::setStartDate(boost::gregorian::date startDate)
  {
    _startDate = startDate;
  }

  boost::posix_time::time_duration Strategy::startTime() const
  {
    return _startTime;
  }

  void Strategy::setStartTime(boost::posix_time::time_duration startTime)
  {
    _startTime = startTime;
  }

  boost::gregorian::date Strategy::endDate() const
  {
    return _endDate;
  }

  void Strategy::setEndDate(boost::gregorian::date endDate)
  {
    _endDate = endDate;
  }

  boost::posix_time::time_duration Strategy::endTime() const
  {
    return _endTime;
  }

  void Strategy::setEndTime(boost::posix_time::time_duration endTime)
  {
    _endTime = endTime;
  }

  boost::gregorian::date Strategy::tExitDate() const
  {
    return _tExitDate;
  }
      
  void Strategy::setTExitDate(boost::gregorian::date tExitDate)
  {
    _tExitDate = tExitDate;
  }

  boost::posix_time::time_duration Strategy::tExitTime() const
  {
    return _tExitTime;
  }

  void Strategy::setTExitTime(boost::posix_time::time_duration tExitTime)
  {
    _tExitTime = tExitTime;
  }

  double Strategy::quantity() const
  {
    return _quantity;
  }

  void Strategy::setQuantity(double quantity)
  {
    _quantity = quantity;
  }

  double Strategy::cancelTicks() const
  {
    return _cancelTicks;
  }

  void Strategy::setCancelTicks(double cancelTicks)
  {
    _cancelTicks = cancelTicks;
  }

  const std::string& Strategy::uploadName() const
  {
    return _uploadName;
  }

  void Strategy::setUploadName(const std::string& uploadName)
  {
    _uploadName = uploadName;
  }

  AlgoStrategyStatus Strategy::strategyStatus() const
  {
    return _strategyStatus;
  }

  void Strategy::setStrategyStatus(AlgoStrategyStatus strategyStatus)
  {
    _strategyStatus = strategyStatus;
  }
} // namespace Db
} // namespace Algo
