/*
 * =====================================================================================
 *
 *       Filename:  StrategyStore.C
 *
 *    Description:
 *
 *        Created:  09/21/2016 12:43:24 PM
 *       Compiler:  g++
 *
 *         Author:  Soumya Prasad Ukil
 *        Company:  AlgoEngine
 *
 * =====================================================================================
 */

#include <shared/common.h>
#include <shared/order.h>
#include <shared/genericstrategy.h>
#include <quote/QuoteBook.H>
#include <fix/FixOPHandler.H>
#include <strategy/AlgoMode1Strategy.H>
#include <storage/StrategyStore.H>
#include <storage/AlertMessageStore.H>
#include <storage/RunningStrategyStore.H>
#include <storage/StoppedStrategyStore.H>
#include <constant/ServerGlobals.H>
#include <shared/easylogging++.h>
#include <strategy/StrategyQueue.H>
#include <memory>
namespace Algo
{
  void StrategyStore::setMaxAttempts(int i)
  {
    LOG(INFO) << "In setMaxAttempts " << (long int)(this);
    _maxAttempts = i;
  }

  void StrategyStore::addStrategy(int id, std::shared_ptr<GenericStrategy> strategy)
  {
    strategy->setMaxAttempts(_maxAttempts);
    RunningStrategyStore::getInstance().addStrategy(id, strategy);
    {
      std::lock_guard<std::mutex> lock(_strategyMapsMutex);
      _runningStrategySet[id] = strategy;
    }
  }

  std::shared_ptr<GenericStrategy> StrategyStore::getStrategy(int i)
  {
    auto itr = _runningStrategySet.find(i);
    if (itr == _runningStrategySet.end())
      return nullptr;
    return itr->second;
  }

  void StrategyStore::removeStrategy(std::map<int, std::shared_ptr<GenericStrategy>>::iterator &itr)
  {
    StoppedStrategyStore::getInstance().addStrategy(itr->first, itr->second);
    RunningStrategyStore::getInstance().removeStrategy(itr->first);
    {
      std::lock_guard<std::mutex> lock(_strategyMapsMutex);
      itr = _runningStrategySet.erase(itr);
    }
  }

  StrategyStore::StrategyStore()
  {
  }

  void StrategyStore::working()
  {
    LOG(INFO) << "In operator " << (long int)(this);
    Order order;
    while (gIsRunning)
    {
      //Check for new strategy.
      if (StrategyQueue::getInstance().isCommandAvailable()) {
        UNSIGNED_CHARACTER commandType = StrategyQueue::getInstance().getCommand();
        switch (commandType)
        {
          case CommandCategory_ALGO_MODEL_1:
            {
              LOG(DEBUG) << "[STRATEGY] Got command for Strategy AlgoModel1 [" << UNSIGNED_SHORT(commandType) << "]";
              char buffer[MAX_BUF];
              if (StrategyQueue::getInstance().getStrategy(buffer, sizeof(AlgoModel1)))
              {
                AlgoModel1 am(buffer + 3);
                if (am.GetStatus() == StrategyStatus_STOPPED) {
                  std::shared_ptr<GenericStrategy> stretgy = getStrategy(am.GetStrategyId());
                  if (stretgy.get() == nullptr) {
                    LOG(WARNING) << "[STRATEGY] Strategy id [" << am.GetStrategyId() << "]does not exits";
                    am.SetStatus(StrategyStatus_UNKNOWN);
                    char buf[MAX_BUF];
                    size_t size = am.Serialize(buf);
                    {
                      Elements e;
                      e._size = size;
                      memcpy(e._clientName, am.GetUserName(), strlen(am.GetUserName()));
                      memcpy(e._elements, buf, size);
                      while (!gSpscStrategyServerMsgQueue.push(e));
                    }
                    break;
                  }
                  stretgy->setCompleted();
                }
                else {
                  std::shared_ptr<AlgoMode1Strategy> strategy = std::make_shared<AlgoMode1Strategy>(am, [] (const Elements &e) { while (!gSpscStrategyServerMsgQueue.push(e)); });
                  std::string orderVenueId = Db::Backend::getInstance().orderVenueId();
                  strategy->setOpHandler(
                      Algo::OrderHandlerMap::getInstance().get(orderVenueId));
                  strategy->setQuotesProvider(QuoteBook::getInstance());
                  addStrategy(am.GetStrategyId(), strategy);
                }
              }
              break;
            }
          case CommandCategory_STOP_STRATEGY:
            {
              LOG(DEBUG) << "[STRATEGY] Got command for stopping Strategy";
              char buffer[MAX_BUF];
              if (StrategyQueue::getInstance().getStrategy(buffer, sizeof(AlgoModelStopStrategy) + 3))
              {
                AlgoModelStopStrategy stopStrategy(buffer + 3);
                std::shared_ptr<GenericStrategy> strategy = getStrategy(stopStrategy.getStrategyId());
                if (strategy.get() == nullptr) {
                    LOG(WARNING) << "[STRATEGY] Strategy id [" << stopStrategy.getStrategyId() << "]does not exits";
                }
                else if (!strategy->isComplete()){
                    LOG(DEBUG) << "[STRATEGY] Stopping strategy ID: " << stopStrategy.getStrategyId();
                    strategy->forceStrategyStop();
                }
              }
              break;
            }
          default:
            LOG(WARNING) << "[STRATEGY] Unhandled strategy type: [" << UNSIGNED_SHORT(commandType) << "]";
            break;
        }
      }

      for (auto itr = _runningStrategySet.begin(); itr != _runningStrategySet.end();) {
        if (itr->second->isComplete()) {
          LOG(DEBUG) << "[STRATEGY] Stopping strategy";
          itr->second->sendCompleted();
          LOG(DEBUG) << "[STRATEGY] Removing from map";
          removeStrategy(itr);
          LOG(DEBUG) << "[STRATEGY] Removed from map";
        }
        else {
          itr->second->processFeed();
          ++itr;
        }
      }
      if (gSpscOrderMsgQueue.pop(order)) {
        std::shared_ptr<GenericStrategy> stretgy = getStrategy(order.getStrategyId());
        if (stretgy.get() == nullptr) {
          LOG(WARNING) << "[STRATEGY] Strategy id [" << order.getStrategyId() << "]does not exits";
        }
        else
        {
          stretgy->processOrder(order);
        }
      }
    }
  }

  int StrategyStore::getNumberOfRunningStrategies() const
  {
    return _runningStrategySet.size();
  }

  void StrategyStore::recoverFromFile()
  {
    LOG(INFO) << "[STRATEGY] recoverFromFile: size of running: : " << _runningStrategySet.size();

    for (auto& runningStrategy: _runningStrategySet)
    {
      LOG(INFO) << "[STRATEGY] recoverFromFile: running: : " << runningStrategy.first;
      runningStrategy.second->setPushToServerMsgQueue([](const Elements &e) { while (!gSpscStrategyServerMsgQueue.push(e)); });
      runningStrategy.second->setAfterRestart();
      std::string orderVenueId = Db::Backend::getInstance().orderVenueId();
      runningStrategy.second->setOpHandler(
          Algo::OrderHandlerMap::getInstance().get(orderVenueId));
      runningStrategy.second->setQuotesProvider(QuoteBook::getInstance());
      Algo::RunningStrategyStore::getInstance().addStrategy(runningStrategy.first, runningStrategy.second);
    }
  }
}
