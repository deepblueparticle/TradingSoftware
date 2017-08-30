/*
 * ==========================================================================================
 *
 *       Filename:  ConnectionStatus.H
 *
 *    Description:  This class holds information about which downstream connection is active.
 *
 *        Created:  09/19/2016 06:45:36 AM
 *       Compiler:  g++
 *
 *         Author:  Soumya Prasad Ukil
 *        Company:  AlgoEngine
 *
 * ==========================================================================================
 */

#ifndef ConnectionStatus_H
#define ConnectionStatus_H

namespace Algo
{
  class ConnectionStatus
  {
    public:
      static ConnectionStatus& getInstance();
      void setFeedConnected(bool val);
      void setOPConnected(bool val);
      bool getFeedConnected();
      bool getOPConnected();

    private:
      ConnectionStatus();

      bool _isFeedConnected;
      bool _isOPConnected;
  };
}

#endif
