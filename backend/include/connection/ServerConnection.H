/*
 * =====================================================================================
 *
 *       Filename:  ServerConnection.H
 *
 *    Description:  
 *
 *        Created:  09/19/2016 11:38:02 AM
 *       Compiler:  g++
 *
 *         Author:  Soumya Prasad Ukil
 *        Company:  AlgoEngine
 *
 * =====================================================================================
 */

#ifndef ServerConnection_H
#define ServerConnection_H

#include <cstdint>
#include <event.h>
#include <event2/event_compat.h>
#include <event2/event.h>
#include <event2/event_struct.h>

namespace Algo
{
  class ServerConnection
  {
    public:
      explicit ServerConnection(int port);
      void operator()();
      static void onHeartbeat(int fd, short ev, void *arg);

    private:
      int setNonBlock(int fd);

      /**
       * This function will be called by libevent when there is a connection
       * ready to be accepted.
       */
      static void onAccept(int fd, short ev, void *arg);
      static void signalCB(evutil_socket_t fd, short event, void *arg);
      void handleAccept(int fd, short ev);
      static void onRead(int fd, short ev, void *arg);

      int _port;
      /* The socket accept event. */
      event* _evAccept;
  };
}

#endif
