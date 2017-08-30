/*
 * =====================================================================================
 *
 *       Filename:  RunningStrategyStore.H
 *
 *    Description:  
 *
 *        Created:  09/20/2016 01:31:29 AM
 *       Compiler:  g++
 *
 *         Author:  Soumya Prasad Ukil
 *        Company:  AlgoEngine
 *
 * =====================================================================================
 */

#ifndef RunningStrategyStore_H
#define RunningStrategyStore_H

#include <stdlib.h>
#include <map>
#include <shared/genericstrategy.h>
#include <strategy/AlgoMode1Strategy.H>

#include <util/SpinLock.H>
#include <boost/serialization/access.hpp>
#include <boost/serialization/map.hpp>

namespace Algo
{
  class RunningStrategyStore
  {
    friend class boost::serialization::access;
    public:
      static RunningStrategyStore& getInstance();
      RunningStrategyStore();
      bool sendRunningStrategy(long index);
      void removeStrategy(long id);
      void addStrategy(long id, std::shared_ptr<GenericStrategy> strategy);
      int numStrategies() { return _runningStrategy.size(); }

    private:
      template<class Archive>
      void serialize(Archive &ar, const unsigned int version)
      {
        ar.register_type(static_cast<AlgoMode1Strategy *>(NULL));
        ar & _runningStrategy;
      }

      SpinLock                                         _runningStrategyLock;
      std::map<long, std::shared_ptr<GenericStrategy>> _runningStrategy;
  };
}

#endif
