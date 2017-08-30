/*
 * =====================================================================================
 *
 *       Filename:  OrderStore.C
 *
 *    Description:  
 *
 *        Created:  09/22/2016 11:46:23 AM
 *       Compiler:  g++
 *
 *         Author:  Soumya Prasad Ukil
 *        Company:  AlgoEngine
 *
 * =====================================================================================
 */

#include <storage/OrderStore.H>
#include <util/SpinLockHelper.H>

namespace Algo
{
  OrderStore& OrderStore::getInstance() 
  {
    static OrderStore e;
    return e;
  }

  void OrderStore::addOrderIntoClOrderIdToOrder(std::string clOrdId, Order *order) 
  {
    SpinLockHelper locker(&_clOrderIdToOrderLock);
    if (_clOrderIdToOrder.find(clOrdId) != _clOrderIdToOrder.end())
      delete _clOrderIdToOrder[clOrdId];
    _clOrderIdToOrder[clOrdId] = order;
  }

  void OrderStore::insertOrder(Order order) 
  {
    SpinLockHelper locker(&_orderVectorLock);
    _orderVector.push_back(order);
  }

  void OrderStore::sortOrderVector() 
  {
    SpinLockHelper locker(&_orderVectorLock);
    std::sort(_orderVector.begin(), _orderVector.end(), OrderStore::compare);
  }

  Order OrderStore::getOrderAt(long index)
  {
    SpinLockHelper locker(&_orderVectorLock);
    return _orderVector.at(index);
  }

  size_t OrderStore::getOrdersCount()
  {
    SpinLockHelper locker(&_orderVectorLock);
    return _orderVector.size();
  }

  Order* OrderStore::getOrderFromClOrderIdToOrder(std::string clOrdId) 
  {
    SpinLockHelper locker(&_clOrderIdToOrderLock);
    auto itr = _clOrderIdToOrder.find(clOrdId);
    if (itr == _clOrderIdToOrder.end())
      return nullptr;
    Order *o = new Order(*(itr->second));
    return o;
  }

  void OrderStore::addStrategyIdIntoClOrderIdToStretgyId(std::string clOrdId, long strategyId) 
  {
    SpinLockHelper locker(&_clOrderIdStrategyIdMapLock);
    _clOrderIdStrategyIdMap[clOrdId] = strategyId;
  }

  bool OrderStore::getStrategyIdFromClOrderIdToStretgyId(std::string clOrdId, long &strategyId) 
  {
    bool ret = false;
    SpinLockHelper locker(&_clOrderIdStrategyIdMapLock);
    auto itr = _clOrderIdStrategyIdMap.find(clOrdId);
    if (itr != _clOrderIdStrategyIdMap.end()) {
      strategyId = itr->second;
      ret = true;
    }
    return ret;
  }

  OrderStore::OrderStore()
  {
  }

  bool OrderStore::compare(const Order& lhs, const Order& rhs)
  {
    return lhs.getExchangeEntryTime() < rhs.getExchangeEntryTime();
  }
}
