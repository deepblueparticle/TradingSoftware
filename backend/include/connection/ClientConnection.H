/*
 * =====================================================================================
 *
 *       Filename:  ClientConnection.H
 *
 *    Description:  
 *
 *        Created:  09/20/2016 01:35:59 AM
 *       Compiler:  g++
 *
 *         Author:  Soumya Prasad Ukil
 *        Company:  AlgoEngine
 *
 * =====================================================================================
 */

#ifndef ClientConnection_H
#define ClientConnection_H

#include <mutex>
#include <cstdint>
#include <event.h>
#include <event2/event_compat.h>
#include <event2/event.h>
#include <event2/event_struct.h>
#include <constant/ServerConstant.H>

namespace Algo
{
  class ClientConnection
  {
    public:
      ClientConnection();
      virtual ~ClientConnection();
      /**
       * This function will be called by libevent when the client socket is
       * ready for reading.
       */
      void onRead(int fd, short ev);
      void connectionHealthCheck(int fd, short ev);
      void setSocketFd(int fd);
      bool write(const char* data, size_t len);
      int  getHeartbeatTimeout();
      long long getLastHeartbeatTime();
      event* getReadEvent();
      void setConnectionStatus(bool);
      bool isConnected();

    private:
      void processBuffer(char *buffer, uint32_t bytesAvailable);
      void handleReadHeader(char *data, size_t bytes_transferred);
      void processLoginMessage(const char* buffer);
      void processHandler(const char* buffer);
      void handleRead(int fd, short ev);
      void handleHealthCheck(short ev);
      void pushScripMasterData();

      size_t      _pendingDataSize;
      char        _pendingBuffer[ServerConstant::MAXDATABUFFER];
      long long   _lastHearbeatTime;
      std::string _clientName;
      int         _socketFd;
      std::mutex  _socketWriteMutex;
      int         _heartbeatTimeout;
      bool        _isConnected;

      /* Events. We need 2 event structures, one for read event
       * notification and the other for writing. */
      event       *_evRead;
      event       *_evConnectionHealthCheck;
      bool        _heartbeatActivated;
  };
}

#endif
