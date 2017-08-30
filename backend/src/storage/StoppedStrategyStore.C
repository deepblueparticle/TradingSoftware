/*
 * =====================================================================================
 *
 *       Filename:  StoppedStrategyStore.C
 *
 *    Description:  
 *
 *        Created:  09/22/2016 08:18:58 AM
 *       Compiler:  g++
 *
 *         Author:  Soumya Prasad Ukil
 *        Company:  AlgoEngine
 *
 * =====================================================================================
 */

#include <shared/easylogging++.h>
#include <storage/StoppedStrategyStore.H>
#include <util/SpinLockHelper.H>

namespace Algo
{
  StoppedStrategyStore& StoppedStrategyStore::getInstance()
  {
    static StoppedStrategyStore s;
    return s;
  }

  StoppedStrategyStore::StoppedStrategyStore()
  {
  }

  size_t StoppedStrategyStore::sendStoppedStrategy(long index){
    bool found = false;
    {
      SpinLockHelper lock(&_stoppedStrategyLock);
      auto itr = _stoppedStrategy.find(index);
      if (itr != _stoppedStrategy.end())
      {
        found = true;
        LOG(DEBUG) << "[STRATEGY] Stopped strategy ["<<index<<"] is sent to front end";
      }
      else
      {
        LOG(INFO) << "[STRATEGY] Strategy [" << index << "] not found in stopped strategy map.";
      }
    }
    return found;
  }

  void StoppedStrategyStore::addStrategy(long id, std::shared_ptr<GenericStrategy> strategy)
  {
    SpinLockHelper lock(&_stoppedStrategyLock);
    _stoppedStrategy[id] = strategy;
  }
}
