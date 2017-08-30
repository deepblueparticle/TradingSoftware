/*
 * =====================================================================================
 *
 *       Filename:  ConnectionManager.H
 *
 *    Description:  
 *
 *        Created:  09/20/2016 01:44:48 AM
 *       Compiler:  g++
 *
 *         Author:  Soumya Prasad Ukil
 *        Company:  AlgoEngine
 *
 * =====================================================================================
 */

#ifndef ConnectionManager_H
#define ConnectionManager_H

#include <string>
#include <map>
#include <connection/ClientConnection.H>

namespace Algo
{
  class ConnectionManager
  {
    public:
      static ConnectionManager& getInstance();
      void removeConnectionForUser(std::string& user);
      void removeConnectionForUser(std::map<std::string, ClientConnection*>::iterator& itr);
      bool addConnectionForUser(std::string& user, ClientConnection* conn);
      std::mutex& getMutex();
      bool isConnected(std::string& user);
      ClientConnection* getConnection(std::string& user);
      std::map<std::string, ClientConnection*>& getPerUserConnectionMap();

    private:
      ConnectionManager();
      ConnectionManager(const ConnectionManager&) = delete;

      std::mutex                               _perUserConnectionMutex;
      std::map<std::string, ClientConnection*> _perUserConnectionMap;
  };
}

#endif
