/*
 * =====================================================================================
 *
 *       Filename:  StoppedStrategyStore.H
 *
 *    Description:  
 *
 *        Created:  09/20/2016 01:27:59 AM
 *       Compiler:  g++
 *
 *         Author:  Soumya Prasad Ukil
 *        Company:  AlgoEngine
 *
 * =====================================================================================
 */

#ifndef StoppedStrategyStore_H
#define StoppedStrategyStore_H

#include <stdlib.h>
#include <map>
#include <shared/genericstrategy.h>
#include <strategy/AlgoMode1Strategy.H>

#include <util/SpinLock.H>
#include <boost/serialization/access.hpp>
#include <boost/serialization/map.hpp>

namespace Algo
{
  class StoppedStrategyStore
  {
    friend class boost::serialization::access;
    public:
      static StoppedStrategyStore& getInstance();
      StoppedStrategyStore();
      size_t sendStoppedStrategy(long index);
      void addStrategy(long id, std::shared_ptr<GenericStrategy> strategy);
      int numStrategies() { return _stoppedStrategy.size(); }

    private:
      template<class Archive>
      void serialize(Archive &ar, const unsigned int version)
      {
        ar.register_type(static_cast<AlgoMode1Strategy *>(NULL));
        ar & _stoppedStrategy;
      }
      SpinLock                                         _stoppedStrategyLock;
      std::map<long, std::shared_ptr<GenericStrategy>> _stoppedStrategy;
  };
}

#endif
