/*
 * =====================================================================================
 *
 *       Filename:  AlertMessageStore.C
 *
 *    Description:  
 *
 *        Created:  09/22/2016 12:04:20 PM
 *       Compiler:  g++
 *
 *         Author:  Soumya Prasad Ukil
 *        Company:  AlgoEngine
 *
 * =====================================================================================
 */

#include <shared/common.h>
#include <storage/AlertMessageStore.H>
#include <constant/ServerGlobals.H>

namespace Algo
{
  AlertMessageStore& AlertMessageStore::getInstance() 
  {
    static AlertMessageStore alertMessageStore;
    return alertMessageStore;
  }

  void AlertMessageStore::addAlert(const Alerts &alert) 
  {
    std::lock_guard<std::mutex> l(_mutex);
    _alertSink.push_back(alert);
  }

  std::mutex& AlertMessageStore::getMutex() 
  {
    return _mutex;
  }

  const std::vector<Alerts>& AlertMessageStore::getSink() 
  {
    return _alertSink;
  }

  void AlertMessageStore::sendAlert(Alerts alert, std::string &user, bool add) 
  {
    char buf[MAX_BUF];
    size_t size = alert.Serialize(buf);
    Elements e;
    e._size = size;
    memcpy(e._clientName, user.c_str(), user.length());
    memcpy(e._elements, buf, size);
    while (!gSpscStrategyServerMsgQueue.push(e));
  }

  AlertMessageStore::AlertMessageStore() : _mutex{}, _alertSink{}
  {
  }
}
