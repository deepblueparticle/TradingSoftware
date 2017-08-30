/*
 * =====================================================================================
 *
 *       Filename:  StrategyStore.H
 *
 *    Description:  
 *
 *        Created:  09/20/2016 01:24:08 AM
 *       Compiler:  g++
 *
 *         Author:  Soumya Prasad Ukil
 *        Company:  AlgoEngine
 *
 * =====================================================================================
 */

#ifndef StrategyStore_H
#define StrategyStore_H

#include <memory>
#include <map>
#include <string>
#include <boost/serialization/access.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <mutex>

#include <shared/genericstrategy.h>
#include <strategy/AlgoMode1Strategy.H>

namespace Algo
{
  class StrategyStore
  {
    friend class boost::serialization::access;
    public:
      void setMaxAttempts(int i);
      void addStrategy(int id, std::shared_ptr<GenericStrategy> strategy);
      std::shared_ptr<GenericStrategy> getStrategy(int i);
      void removeStrategy(std::map<int, std::shared_ptr<GenericStrategy>>::iterator &itr);
      explicit StrategyStore();
      void working();
      int getNumberOfRunningStrategies() const;
      void recoverFromFile();
    private:
      template<class Archive>
      void serialize(Archive &ar, const unsigned int version)
      {
        ar.register_type(static_cast<AlgoMode1Strategy *>(NULL));
        {
          std::lock_guard<std::mutex> lock(_strategyMapsMutex);
          ar & _runningStrategySet;
        }
        ar & _maxAttempts; 
        ar & _symbolMap; 
      }
      std::map<int, std::shared_ptr<GenericStrategy>> _runningStrategySet;
      int                                             _maxAttempts;
      std::map<std::string, int>                      _symbolMap;
      std::mutex                                      _strategyMapsMutex;
  };
}

#endif
