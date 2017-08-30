#include <interactive_brokers/ConnectionHandler.H>

#include <ib_api/client/EWrapper.h>
#include <ib_api/client/EReader.h>
#include <ib_api/client/EClientSocket.h>
#include <shared/easylogging++.h>

namespace Algo
{
namespace Ib
{

ConnectionHandler::ConnectionHandler()
    : _osSignal(2000),  // 2-seconds timeout
      _client(new EClientSocket(this, &_osSignal)),
      _reader(nullptr) {}

ConnectionHandler::~ConnectionHandler() {
  if (_reader) {
    delete _reader;
  }

  delete _client;
}

bool ConnectionHandler::connect(const std::string& host, const int port) {
  static int sClientId = 1;

  LOG(TRACE) << "Connecting to " << host << ":" << port;

  // Each running connection to IB Gateway should have unique client id
  // Basically we will have just two ids for Market data connection and orders.
  bool status = _client->eConnect(host.c_str(), port, sClientId);
  if (!status) {
    LOG(ERROR) << "Cannot connect to " << host << ":" << port;
    return false;
  }
  LOG(TRACE) << "Connected to " << host << ":" << port;

  ++sClientId;

  _reader = new EReader(_client, &_osSignal);
  _reader->start();

  return status;
}

void ConnectionHandler::disconnect() const
{
  _client->eDisconnect();
  LOG(TRACE) << "Disconnect";
}

bool ConnectionHandler::isConnected() const
{
  return _client->isConnected();
}

void ConnectionHandler::processMessages()
{
  _reader->checkClient();
  _osSignal.waitForSignal();
  _reader->processMsgs();
}

void ConnectionHandler::error(const int id, const int errorCode,
                              const std::string errorStr) {
  LOG(ERROR) << "Id: " << id << " Code: " << errorCode << " Msg: " << errorStr;
}

}
}
