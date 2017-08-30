/*
 * =====================================================================================
 *
 *       Filename:  ServerConnection.C
 *
 *    Description:  
 *
 *        Created:  09/22/2016 11:11:01 PM
 *       Compiler:  g++
 *
 *         Author:  Soumya Prasad Ukil
 *        Company:  AlgoEngine
 *
 * =====================================================================================
 */

#include <arpa/inet.h>
#include <shared/easylogging++.h>
#include <connection/ServerConnection.H>
#include <connection/ClientConnection.H>
#include <constant/ServerGlobals.H>
#include <fcntl.h>

namespace Algo
{
  ServerConnection::ServerConnection(int port) : _port{port}, _evAccept{nullptr}
  {
  }

  void ServerConnection::operator()()
  {
    _evAccept = new event();
    /* Initalize the event library */
    event_base *base = event_init();

    int listenFd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenFd < 0)
    {
      LOG(ERROR) << "listen failed";
    }
    int reuseaddr_on = 1;
#ifdef __linux__
    if (setsockopt(listenFd, SOL_SOCKET, SO_REUSEADDR, &reuseaddr_on,
          sizeof(reuseaddr_on)) == -1)
#else if __Win32
      if (setsockopt(listenFd, SOL_SOCKET, SO_REUSEADDR, (char*)&reuseaddr_on,
            sizeof(reuseaddr_on)) == -1)
#endif
      {
        LOG(ERROR) << "setsockopt failed";
      }
    struct sockaddr_in listenAddr;
    memset(&listenAddr, 0, sizeof(listenAddr));
    listenAddr.sin_family = AF_INET;
    listenAddr.sin_addr.s_addr = INADDR_ANY;
    listenAddr.sin_port = htons(_port);
    if (bind(listenFd, (struct sockaddr *)&listenAddr,
          sizeof(listenAddr)) < 0)
    {
      LOG(ERROR)<<"bind failed";
    }
    if (listen(listenFd, 5) < 0)
    {
      LOG(ERROR)<<"listen failed";
    }
    /* Set the socket to non-blocking, this is essential in event
     * based programming with libevent. */
    if (setNonBlock(listenFd) < 0)
    {
      LOG(ERROR)<<"failed to set server socket to non-blocking";
    }
#ifdef _WIN32
    event_set(_evAccept, (evutil_socket_t)listenFd, EV_READ|EV_PERSIST, Server::OnAccept, NULL);//event_self_cbarg());
#else
    event_set(_evAccept, listenFd, EV_READ|EV_PERSIST, ServerConnection::onAccept, NULL);//event_self_cbarg());
    struct event *signalInt = evsignal_new(base, SIGINT, ServerConnection::signalCB, base);
    event_add(signalInt, NULL);
#endif
    event_add(_evAccept, NULL);
    event_dispatch();
  }

  void ServerConnection::onAccept(int fd, short ev, void *arg)
  {
    ServerConnection *obj = static_cast<ServerConnection*>(arg);
    obj->handleAccept(fd, ev);
  }

  int ServerConnection::setNonBlock(int fd)
  {
#ifdef _WIN32
    unsigned long ul;
    ioctlsocket(fd, FIONBIO, &ul);
#else
    fcntl(fd, F_SETFL, O_NONBLOCK);
#endif
    return 0;
  }

  void ServerConnection::signalCB(evutil_socket_t fd, short event, void *arg)
  {
    event_base *base = static_cast<event_base*>(arg);
    gIsRunning = false;
    event_base_loopbreak(base);
  }

  void ServerConnection::handleAccept(int fd, short ev)
  {
    int clientFd;
    sockaddr_in clientAddr;
    socklen_t clientLen = sizeof(clientAddr);

    /* Accept the new connection. */
    clientFd = accept(fd, (struct sockaddr *)&clientAddr, &clientLen);
    if (clientFd == -1)
    {
      LOG(ERROR)<<"[SERVER] accept failed";
      return;
    }

    /* Set the client socket to non-blocking mode. */
    if (setNonBlock(clientFd) < 0)
    {
      LOG(ERROR)<<"[SERVER] failed to set client socket non-blocking";
    }
        /* We've accepted a new client, allocate a client object to
         * maintain the state of this client. */
    ClientConnection* client = new ClientConnection();
    if (!client)
    {
      LOG(ERROR)<<"[SERVER] New failed in ClientConn";
      return;
    }

    /* Setup the read event, libevent will call on_read() whenever
     * the clients socket becomes read ready.  We also make the
     * read event persistent so we don't have to re-add after each
     * read. */

#ifdef _WIN32
    event_set(client->getReadEvent(), (evutil_socket_t)clientFd, EV_READ|EV_PERSIST, ClientConnection::OnRead, client);
#else
    event_set(client->getReadEvent(), clientFd, EV_READ|EV_PERSIST, ServerConnection::onRead, client);
#endif
    if(!client->getReadEvent())
    {
      LOG(ERROR)<<"event_assign failed with "<<strerror(errno);
#ifdef _WIN32
      CloseHandle((HANDLE)clientFd);
#else
      close(clientFd);
#endif
      return;
    }
    client->setSocketFd(clientFd);
    /* Setting up the event does not activate, add the event so it
     * becomes active. */
    if (event_add(client->getReadEvent(), NULL) == -1)
    {
      LOG(ERROR)<<"Failed to add event for "<<clientFd ;
#ifdef _WIN32
      CloseHandle((HANDLE)clientFd);
#else
      close(clientFd);
#endif
    }
    client->setConnectionStatus(true);
    LOG(INFO)<<"Accepted connection from "<< inet_ntoa(clientAddr.sin_addr) << " FD " << clientFd;
  }

  void ServerConnection::onRead(int fd, short ev, void *arg)
  {
    ClientConnection *client = static_cast<ClientConnection*>(arg);
    client->onRead(fd, ev);
    if (!client->isConnected())
    {
      LOG(ERROR)<<"Deleting client "<< fd << " from backend";
      delete client;
    }
  }

  void ServerConnection::onHeartbeat(int fd, short ev, void *arg)
  {
    ClientConnection *client = static_cast<ClientConnection*>(arg);
    client->connectionHealthCheck(fd, ev);
    if (!client->isConnected())
    {
      LOG(ERROR)<<"Deleting client "<< fd << " from backend";
      delete client;
    }
  }
}
