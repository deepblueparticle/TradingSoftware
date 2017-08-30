/*
 * =====================================================================================
 *
 *       Filename:  RunningStrategyStore.C
 *
 *    Description:  
 *
 *        Created:  09/22/2016 10:43:05 AM
 *       Compiler:  g++
 *
 *         Author:  Soumya Prasad Ukil
 *        Company:  AlgoEngine
 *
 * =====================================================================================
 */

#include <shared/easylogging++.h>
#include <storage/RunningStrategyStore.H>
#include <util/SpinLockHelper.H>

namespace Algo
{
  RunningStrategyStore& RunningStrategyStore::getInstance()
  {
    static RunningStrategyStore s;
    return s;
  }

  RunningStrategyStore::RunningStrategyStore() 
  {
  }

  bool RunningStrategyStore::sendRunningStrategy(long index){
    bool found = false;
    {
      SpinLockHelper lock(&_runningStrategyLock);
      auto itr = _runningStrategy.find(index);
      if (itr != _runningStrategy.end())
      {
        found = true;
        LOG(DEBUG) << "[STRATEGY] Running strategy ["<<index<<"] is sent to front end";
      }
      else
      {
        LOG(INFO) << "[STRATEGY] Strategy ["<<index<<"] not found in running strategy map.";
      }
    }
    return found;
  }

  void RunningStrategyStore::removeStrategy(long id)
  {
    SpinLockHelper lock(&_runningStrategyLock);
    _runningStrategy.erase(id);
  }

  void RunningStrategyStore::addStrategy(long id, std::shared_ptr<GenericStrategy> strategy)
  {
    SpinLockHelper lock(&_runningStrategyLock);
    _runningStrategy[id] = strategy;
  }
}
