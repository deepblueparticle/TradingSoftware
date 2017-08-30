/*
 * =====================================================================================
 *
 *       Filename:  ConnectionManager.C
 *
 *    Description:  
 *
 *        Created:  09/23/2016 03:57:44 AM
 *       Compiler:  g++
 *
 *         Author:  Soumya Prasad Ukil
 *        Company:  AlgoEngine
 *
 * =====================================================================================
 */

#include <connection/ConnectionManager.H>

namespace Algo
{
  ConnectionManager& ConnectionManager::getInstance()
  {
    static ConnectionManager conn;
    return conn;
  }

  void ConnectionManager::removeConnectionForUser(std::string& user)
  {
    _perUserConnectionMap.erase(user);
  }

  void ConnectionManager::removeConnectionForUser(std::map<std::string, ClientConnection*>::iterator& itr)
  {
    itr = _perUserConnectionMap.erase(itr);
  }

  bool ConnectionManager::addConnectionForUser(std::string& user, ClientConnection* connection)
  {
    return _perUserConnectionMap.insert(std::pair<std::string, ClientConnection*>(user, connection)).second;
  }

  std::mutex& ConnectionManager::getMutex()
  {
    return _perUserConnectionMutex;
  }

  bool ConnectionManager::isConnected(std::string& user)
  {
    return _perUserConnectionMap.find(user) != _perUserConnectionMap.end();
  }

  ClientConnection* ConnectionManager::getConnection(std::string& user)
  {
    return _perUserConnectionMap.find(user)->second;
  }

  std::map<std::string, ClientConnection*>& ConnectionManager::getPerUserConnectionMap() 
  { 
    return _perUserConnectionMap; 
  }

  ConnectionManager::ConnectionManager()
  {
  }
}
