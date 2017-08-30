/*
 * =====================================================================================
 *
 *       Filename:  ServerGlobals.C
 *
 *    Description:
 *
 *        Created:  09/22/2016 12:38:35 PM
 *       Compiler:  g++
 *
 *         Author:  Soumya Prasad Ukil
 *        Company:  AlgoEngine
 *
 * =====================================================================================
 */

#include <shared/order.h>
#include <shared/common.h>
#include <constant/ServerGlobals.H>

namespace Algo
{
  LockfreeSpscQueue<Algo::Elements, 1024> gSpscStrategyServerMsgQueue;
  LockfreeSpscQueue<Algo::Order, 1024> gSpscOrderMsgQueue;
  LockfreeSpscQueue<std::pair<DBOperation, std::shared_ptr<Db::InternalEvent>>, 1024> gInternalEventsQueue;
  LockfreeSpscQueue<std::pair<DBOperation, std::shared_ptr<Db::RejectEvent>>, 1024> gRejectEventsQueue;
  LockfreeSpscQueue<std::pair<DBOperation, std::shared_ptr<Db::IncomingMessageEvent>>, 1024> gIncomingMessageEventsQueue;
  LockfreeSpscQueue<std::pair<DBOperation, std::shared_ptr<Db::Order>>, 1024> gOrdersQueue;
  LockfreeSpscQueue<std::pair<DBOperation, std::shared_ptr<Db::Strategy>>, 1024> gStrategiesQueue;
  bool gIsRunning = true;
  std::map<std::string, int> openStrategyAllowanceMap;
}
