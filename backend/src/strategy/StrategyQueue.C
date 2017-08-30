/*
 * =====================================================================================
 *
 *       Filename:  StrategyQueue.C
 *
 *    Description:  
 *
 *        Created:  09/23/2016 06:01:48 AM
 *       Compiler:  g++
 *
 *         Author:  Soumya Prasad Ukil
 *        Company:  AlgoEngine
 *
 * =====================================================================================
 */

#include <shared/easylogging++.h>
#include <strategy/StrategyQueue.H>

namespace Algo
{
  StrategyQueue& StrategyQueue::getInstance()
  {
    static StrategyQueue strategyQueue;
    return strategyQueue;
  }

  void StrategyQueue::addCommand(UNSIGNED_CHARACTER command)
  {
    LOG(INFO) << "[STRATEGY] Adding Command ";
    while (!_commandQueue.push(command));
    LOG(INFO) << "[STRATEGY] Added Command ";

  }

  bool StrategyQueue::isCommandAvailable()
  {
    return !_commandQueue.empty();
  }

  UNSIGNED_CHARACTER StrategyQueue::getCommand()
  {
    LOG(INFO) << "[STRATEGY] Getting Command";
    UNSIGNED_CHARACTER command;
    while(!_commandQueue.pop(command));
    return command;
  }

  void StrategyQueue::addStrategy(const char *str, size_t size)
  {
    int s = _strategyQueue.push(str, size);
    LOG(INFO) << "[STRATEGY] addStrategy returned ["<< s <<"] bytes";
  }

  bool StrategyQueue::isStrategyAvailable()
  {
    return _strategyQueue.read_available();
  }

  bool StrategyQueue::getStrategy(char *t, size_t size)
  {
    int s = _strategyQueue.pop(t, size);
    LOG(INFO) << "[STRATEGY] getStrategy returned ["<< s <<"] bytes";
    return true;
  }

  StrategyQueue::StrategyQueue()
  {
  }
}
