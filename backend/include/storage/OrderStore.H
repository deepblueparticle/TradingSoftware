/*
 * =====================================================================================
 *
 *       Filename:  OrderStore.H
 *
 *    Description:  
 *
 *        Created:  09/20/2016 01:02:57 AM
 *       Compiler:  g++
 *
 *         Author:  Soumya Prasad Ukil
 *        Company:  AlgoEngine
 *
 * =====================================================================================
 */

#ifndef OrderStore_H
#define OrderStore_H

#include <shared/order.h>
#include <unordered_map>
#include <util/SpinLock.H>

namespace Algo
{
  class OrderStore
  {
    public:
      static OrderStore &getInstance();
      void addOrderIntoClOrderIdToOrder(std::string clOrdId, Order *order);
      void insertOrder(Order order);
      void sortOrderVector();
      Order getOrderAt(long index);
      size_t getOrdersCount();
      Order *getOrderFromClOrderIdToOrder(std::string clOrdId);
      void addStrategyIdIntoClOrderIdToStretgyId(std::string clOrdId, long strategyId);
      bool getStrategyIdFromClOrderIdToStretgyId(std::string clOrdId, long &strategyId);

    private:
      OrderStore();
      OrderStore(const OrderStore&) = delete;
      OrderStore& operator=(const OrderStore&) = delete;
      static bool compare(const Order& lhs, const Order& rhs);

      std::unordered_map<std::string, Order*>      _clOrderIdToOrder;
      SpinLock                                     _clOrderIdToOrderLock;
      std::unordered_map<std::string, long>        _clOrderIdStrategyIdMap;
      SpinLock                                     _clOrderIdStrategyIdMapLock;
      std::unordered_map<std::string, std::string> _exchangeOrderClOrderIdMap;
      SpinLock                                     _orderVectorLock;
      std::vector<Order>                           _orderVector;
  };
}

#endif
