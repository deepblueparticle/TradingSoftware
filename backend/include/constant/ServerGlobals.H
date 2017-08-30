/*
 * =====================================================================================
 *
 *       Filename:  ServerGlobals.H
 *
 *    Description:  Service side global variables which are used across the application.
 *
 *        Created:  09/22/2016 01:33:24 AM
 *       Compiler:  g++
 *
 *         Author:  Soumya Prasad Ukil
 *        Company:  AlgoEngine
 *
 * =====================================================================================
 */

#ifndef ServerGlobals_H
#define ServerGlobals_H

#include <boost/lockfree/spsc_queue.hpp>

#include <shared/SingletonObjectStorageMap.h>
#include <shared/imarket_data_handler.h>
#include <shared/iophandler.h>
#include <constant/DBOperation.H>
#include <db/IncomingMessageEvent.H>
#include <db/IncomingMessageEvent_odb.H>
#include <db/InternalEvent.H>
#include <db/InternalEvent_odb.H>
#include <db/RejectEvent.H>
#include <db/RejectEvent_odb.H>
#include <db/Order.H>
#include <db/Order_odb.H>
#include <db/Strategy.H>
#include <db/Strategy_odb.H>
#include <atomic>

namespace Algo
{
#ifdef __linux__
  struct Elements;
  class Order;
#endif

  template <class T, size_t size>
  using LockfreeSpscQueue =
    boost::lockfree::spsc_queue<T, boost::lockfree::capacity<size>>;

  extern LockfreeSpscQueue<Algo::Elements, 1024> gSpscStrategyServerMsgQueue;
  extern LockfreeSpscQueue<Algo::Order, 1024> gSpscOrderMsgQueue;

  extern LockfreeSpscQueue<std::pair<DBOperation, std::shared_ptr<Db::InternalEvent>>, 1024> gInternalEventsQueue;
  extern LockfreeSpscQueue<std::pair<DBOperation, std::shared_ptr<Db::RejectEvent>>, 1024> gRejectEventsQueue;
  extern LockfreeSpscQueue<std::pair<DBOperation, std::shared_ptr<Db::IncomingMessageEvent>>, 1024> gIncomingMessageEventsQueue;
  extern LockfreeSpscQueue<std::pair<DBOperation, std::shared_ptr<Db::Order>>, 1024> gOrdersQueue;
  extern LockfreeSpscQueue<std::pair<DBOperation, std::shared_ptr<Db::Strategy>>, 1024> gStrategiesQueue;

  extern bool gIsRunning;

  typedef SingletonObjectStorageMap<IMarketDataHandler> MarketDataHandlerMap;
  typedef SingletonObjectStorageMap<IOpHandler> OrderHandlerMap;

  extern std::map<std::string, int> openStrategyAllowanceMap;
}

#endif
