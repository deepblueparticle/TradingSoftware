/*
 * =====================================================================================
 *
 *       Filename:  ConnectionStatus.C
 *
 *    Description:  
 *
 *        Created:  09/22/2016 11:03:31 PM
 *       Compiler:  g++
 *
 *         Author:  Soumya Prasad Ukil
 *        Company:  AlgoEngine
 *
 * =====================================================================================
 */

#include <connection/ConnectionStatus.H>

namespace Algo
{
  ConnectionStatus& ConnectionStatus::getInstance()
  {
    static ConnectionStatus c;
    return c;
  }

  void ConnectionStatus::setFeedConnected(bool val) 
  { 
    _isFeedConnected = val;
  }

  void ConnectionStatus::setOPConnected(bool val) 
  { 
    _isOPConnected = val;
  }

  bool ConnectionStatus::getFeedConnected() 
  { 
    return _isFeedConnected;
  }

  bool ConnectionStatus::getOPConnected() 
  { 
    return _isOPConnected;
  }

  ConnectionStatus::ConnectionStatus()
  {
  }
}
