/*
 * =====================================================================================
 *
 *       Filename:  StrategyQueue.H
 *
 *    Description:  
 *
 *        Created:  09/18/2016 03:09:00 AM
 *       Compiler:  g++
 *
 *         Author:  Soumya Prasad Ukil
 *        Company:  AlgoEngine
 *
 * =====================================================================================
 */

#ifndef StrategyQueue_H
#define StrategyQueue_H

#include <boost/lockfree/spsc_queue.hpp>
#include <boost/lockfree/queue.hpp>
#include <shared/defines.h>

namespace Algo
{
  class StrategyQueue
  {
    public:
      static StrategyQueue& getInstance();
      void addCommand(UNSIGNED_CHARACTER command);
      bool isCommandAvailable();
      UNSIGNED_CHARACTER getCommand();
      void addStrategy(const char* str, size_t size);
      bool isStrategyAvailable();
      bool getStrategy(char* t, size_t size);

    private:
      StrategyQueue();
      boost::lockfree::spsc_queue<char, boost::lockfree::capacity<1000000>> _strategyQueue;
      boost::lockfree::queue<char, boost::lockfree::capacity<1000>>         _commandQueue;
  };
}

#endif
