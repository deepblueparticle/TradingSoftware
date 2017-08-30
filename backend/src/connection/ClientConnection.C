/*
 * =====================================================================================
 *
 *       Filename:  ClientConnection.C
 *
 *    Description:  
 *
 *        Created:  09/23/2016 03:05:07 AM
 *       Compiler:  g++
 *
 *         Author:  Soumya Prasad Ukil
 *        Company:  AlgoEngine
 *
 * =====================================================================================
 */

#include <cstring>
#include <unistd.h>
#include <shared/commands.h>
#include <shared/response.h>
#include <shared/easylogging++.h>
#include <shared/defines.h>
#include <connection/ServerConnection.H>
#include <connection/ClientConnection.H>
#include <connection/ConnectionManager.H>
#include <config/Configuration.H>
#include <connection/DatabaseConnection.H>
#include <connection/ConnectionStatus.H>
#include <storage/OrderStore.H>
#include <strategy/StrategyQueue.H>
#include <storage/RunningStrategyStore.H>
#include <storage/StoppedStrategyStore.H>
#include <storage/AlertMessageStore.H>
#include <symbol/SymbolDictionary.H>
#include <symbol/InstrumentMap.H>
#include <constant/ServerGlobals.H>
#include <db/Backend_odb.H>

namespace Algo
{
  ClientConnection::ClientConnection() : _lastHearbeatTime(time(0)),_heartbeatActivated(false), _isConnected(false)
  {
    _evRead = new event();
    _evConnectionHealthCheck = new event();
    memset(_pendingBuffer, 0, ServerConstant::MAXDATABUFFER);
    _pendingDataSize = 0;
  }

  ClientConnection::~ClientConnection()
  {
    if(_evRead)
    {
      event_del(_evRead);
      delete _evRead;
    }
    if(_evConnectionHealthCheck && _heartbeatActivated)
    {
      event_del(_evConnectionHealthCheck);
      delete _evConnectionHealthCheck;
    }
    else
    {
      delete _evConnectionHealthCheck;
    }
  }

  void ClientConnection::onRead(int fd, short ev)
  {
    this->handleRead(fd, ev);
  }

  void ClientConnection::connectionHealthCheck(int fd, short ev)
  {
    this->handleHealthCheck(ev);
  }

  void ClientConnection::setSocketFd(int fd) 
  {
    _socketFd = fd; 
  }

  bool ClientConnection::write(const char* data, size_t len)
  {
    std::lock_guard<std::mutex> l(_socketWriteMutex);
    size_t ret_val;
#ifdef _WIN32
    ret_val = send(_socketFd, data, len, 0);
#else
    ret_val = ::write(_socketFd, data, len);
#endif
    LOG(INFO) << "[SERVER] Data Written to client [" << ret_val << "] bytes";
    return  ret_val == len;
  }

  int ClientConnection::getHeartbeatTimeout() 
  { 
    return _heartbeatTimeout; 
  }

  long long ClientConnection::getLastHeartbeatTime() 
  { 
    return _lastHearbeatTime; 
  }

  event* ClientConnection::getReadEvent()
  {
    return _evRead;
  }

  void ClientConnection::setConnectionStatus(bool isConnected_)
  {
    _isConnected = isConnected_;
  }

  bool ClientConnection::isConnected()
  {
    return _isConnected;
  }

  void ClientConnection::processBuffer(char *buffer, uint32_t bytesAvailable)
  {
    //LOG(INFO) << "[Server] In Process Buffer total available data [" << bytesAvailable <<"] bytes";
    uint16_t tmp1 = 0, tmp2 = 0;
    uint16_t packetSize = 0;
    uint16_t initialSize = bytesAvailable;
    uint16_t currentBuffPos =  0;
    while(bytesAvailable>0)
    {
      DESERIALIZE_16(tmp1, tmp2, packetSize = tmp2, buffer, currentBuffPos);
      //LOG(INFO) << "[Server] Packet Size = " << packetSize;
      if(bytesAvailable && bytesAvailable < uint16_t(packetSize+2))
      {
        memcpy (_pendingBuffer, buffer+currentBuffPos-2, bytesAvailable);
        _pendingDataSize = bytesAvailable;// - (currentBuffPos-2);
        break;
      }
      processHandler(buffer+currentBuffPos-2);
      currentBuffPos = currentBuffPos + packetSize;
      bytesAvailable = initialSize - currentBuffPos;
    }
    if(bytesAvailable <= 0)
    {
      memset(_pendingBuffer, 0, ServerConstant::MAXDATABUFFER);
      _pendingDataSize = 0;
    }
  }

  void ClientConnection::handleReadHeader(char *data, size_t bytesTransferred)
  {
    //LOG(INFO) << "[Server] In read Handler ";
    char buffer[ServerConstant::MAXDATABUFFER];
    memset(buffer, 0, ServerConstant::MAXDATABUFFER);
    if(_pendingDataSize > 0)
    {
      memcpy(buffer, _pendingBuffer, _pendingDataSize);
      memcpy(buffer+_pendingDataSize, data, bytesTransferred);
      bytesTransferred += _pendingDataSize;
      //LOG(INFO) << "[Server] Pending data [" << _pendingDataSize <<"] bytes";
      //LOG(INFO) << "[Server] Bytes Transferred to client [" << bytesTransferred <<"] bytes";
      processBuffer(buffer, bytesTransferred);
    }
    else
    {
      memcpy(buffer, data, bytesTransferred);
      processBuffer(buffer, bytesTransferred);
    }
  }

  //TODO: Read password from a file and generate random password salt and add authentication based on that
  void ClientConnection::processLoginMessage(const char* buffer)
  {
    Login login(buffer);
    char responseBuffer[500];
    if (login.IsSaltRequest() == 0)
    {
      std::string finalpassword("4d44c4cb7f256b6597e1d02c470178a3bdc5e7e6"); 
      LOG(INFO) << "[SERVER] Got password [" << login.GetPassword() << "]";
      if (!strcmp(login.GetPassword(), finalpassword.c_str()))
      {
        LOG(INFO) << "[SERVER] Login Success";
        ConnectionRsp connectionResponse;
        connectionResponse.setResponseType(ResponseCategory_CONNECTION);
        connectionResponse.setLoginResponse((UNSIGNED_CHARACTER)LoginResponse_SUCCESS);
        connectionResponse.setFirstStrategyId(getFirstStrategyId());
        connectionResponse.setBackendId(Algo::Db::Backend::getInstance().id());
        _heartbeatTimeout = login.GetHeartbeatTimeout();
        _lastHearbeatTime = time(0);
        LOG(INFO) << "[SERVER] Heartbeat timeout for user " << login.GetUserName() << " is " << _heartbeatTimeout;
        _clientName = std::string(login.GetUserName());
        {
          std::lock_guard<std::mutex> l(ConnectionManager::getInstance().getMutex());
          ConnectionManager::getInstance().addConnectionForUser(_clientName, this);
          LOG(INFO) << "[SERVER] User " << _clientName << " Added in connection map";
        }

        struct timeval tv;

        tv.tv_sec = _heartbeatTimeout / 2;
        tv.tv_usec = 0;
        event_set(_evConnectionHealthCheck, 0, EV_READ | EV_PERSIST, ServerConnection::onHeartbeat, this);
        _heartbeatActivated = true;
        evtimer_add(_evConnectionHealthCheck, &tv);

        size_t bytes = connectionResponse.serialize(responseBuffer);
        write(responseBuffer, bytes);
        connectionResponse.dump();
      }
      //TODO: Send message to GUI wrong password...
      else // Wrong password close connection
      {
        ConnectionRsp connectionResponse;
        connectionResponse.setResponseType(ResponseCategory_CONNECTION);
        connectionResponse.setLoginResponse((UNSIGNED_CHARACTER)LoginResponse_BAD_PASSWORD);

        size_t bytes = connectionResponse.serialize(responseBuffer);
        write(responseBuffer, bytes);
        LOG(INFO) << "[SERVER] Closing connection bad password.";
#ifdef _WIN32
        CloseHandle((HANDLE)_socketFd);
#else
        close(_socketFd);
#endif
        this->setConnectionStatus(false);
        return;
      }
    }
    else
    {
      {
        std::lock_guard<std::mutex> l(ConnectionManager::getInstance().getMutex());
        std::string user(login.GetUserName());
        if (ConnectionManager::getInstance().isConnected(user))
        {
          LOG(INFO) << "Client " << login.GetUserName() << " already logged in multiple login is not allowed.";
          ConnectionRsp connectionResponse;
          connectionResponse.setResponseType(ResponseCategory_CONNECTION);
          connectionResponse.setLoginResponse((UNSIGNED_CHARACTER)LoginResponse_MULTIPLE_LOGINS);
          size_t bytes = connectionResponse.serialize(responseBuffer);
          write(responseBuffer, bytes);
#ifdef _WIN32
          CloseHandle((HANDLE)_socketFd);
#else
          close(_socketFd);
#endif
          //event_del(_evRead);
          this->setConnectionStatus(false);
          return;
        }
      }
      LOG(INFO) << "[Server] Salt Req Password ";
      ConnectionRsp connectionResponse;
      connectionResponse.setResponseType(ResponseCategory_CONNECTION);
      connectionResponse.setPasswordSalt("0047b33a9a3bf17849893ce9c1169e3f4bf94e08");
      connectionResponse.setLoginResponse((UNSIGNED_CHARACTER)LoginResponse_SUCCESS);
      size_t bytes = connectionResponse.serialize(responseBuffer);
      write(responseBuffer, bytes);
    }
  }

  void ClientConnection::processHandler(const char* buffer)
  {
    // First Category of response, then confirmation
    uint8_t commandCategory;
    u_char tmpChar = 0;
    uint32_t offset = 2;

    // De-Serialize command category
    DESERIALIZE_8(tmpChar, commandCategory = tmpChar, buffer, offset);
    LOG(INFO) << "[SERVER] Command received " << int(commandCategory);
    switch (commandCategory)
    {
      case CommandCategory::CommandCategory_LOGIN:
        {
          LOG(INFO) << "[SERVER] Login Command received";
          processLoginMessage(buffer + offset);
        }
        break;
      case CommandCategory_SEND_SCRIP_MASTER_DATA:
        LOG(INFO) << "[SERVER] Received CommandCategory_SEND_SCRIP_MASTER_DATA";
        pushScripMasterData();
        break;
      case CommandCategory_ALGO_MODEL_1:
        {
          LOG(INFO) << "[SERVER] Received strategy Req";
          AlgoModel1 am(buffer + offset);
          if (am.GetStrategyId() == 0)
          {
            LOG(INFO) << "[SERVER] Adding new strategy";
            am.SetStrategyId(generateUniqueStrategyId());
          }
          am.SetUserName(_clientName.c_str());
          am.Dump(" [SERVER] ");
          char buf[MAX_BUF];
          am.Serialize(buf);
          std::string dataFeedVenueId = Db::Backend::getInstance().dataFeedVenueId();
          IMarketDataHandler *marketDataHandlerPtr = MarketDataHandlerMap::getInstance()
                                                     .get(dataFeedVenueId);
          Db::Instrument dataInstrument(am.GetDataSymbol(), am.GetDataSecType(), 
                                        am.GetDataExchange(), am.GetDataCurrency(), dataFeedVenueId);
          int dataSymbolId = InstrumentMap::getInstance().getSymbolId(dataInstrument);
          if (dataSymbolId == -1)
          {
            DatabaseConnection db;
            Configuration* config = Configuration::getInstance();
            bool dbConnected = db.startConnection(
                                 config->getCentralDbUser(),
                                 config->getCentralDbPassword(),
                                 config->getCentralDbName(),
                                 config->getCentralDbAddress(),
                                 config->getCentralDbPort()
                                 );
            if (dbConnected)
            {
              bool success = db.persistInOneTransaction(dataInstrument);
              if (success)
              {
                LOG(INFO) << "Symbol added. " << dataInstrument << " ID: " << dataInstrument.id();
                InstrumentMap::getInstance().pushSymbol(dataInstrument.id(), dataInstrument);
              }
              else
              {
                LOG(ERROR) << "Could not add data symbol '"
                           << dataInstrument << "' in db. Dropping strategy: "
                           << am.GetStrategyId();
                return;
              }
            }
          }
          if (!marketDataHandlerPtr->subscribe(dataInstrument)) 
          {
            LOG(ERROR) << "Failed to subscribe market data for data symbol '" << dataInstrument
                       << "'. Dropping strategy: " << am.GetStrategyId();
          }
          else
          {
            StrategyQueue::getInstance().addStrategy(buf, sizeof(AlgoModel1));
            StrategyQueue::getInstance().addCommand((UNSIGNED_CHARACTER)CommandCategory_ALGO_MODEL_1);
          }
          break;
        }
      case CommandCategory_STOP_STRATEGY:
        {
          LOG(INFO) << "[SERVER] Received stop strategy Req";
          AlgoModelStopStrategy stopStrategy(buffer + offset);
          LOG(INFO) << "[SERVER] StrategyID: " << stopStrategy.getStrategyId();
          char buf[MAX_BUF];
          int size = stopStrategy.serialize(buf);
          StrategyQueue::getInstance().addStrategy(buf, size);
          StrategyQueue::getInstance().addCommand((UNSIGNED_CHARACTER)CommandCategory_STOP_STRATEGY);
          break;
        }
      case CommandCategory_CONNECTION:
        {
          LOG(INFO) << "[SERVER] Received Heartbeat";
          _lastHearbeatTime = std::time(nullptr);
          std::string dataFeedVenueId = Db::Backend::getInstance().dataFeedVenueId();
          std::string orderVenueId = Db::Backend::getInstance().orderVenueId();
          Heartbeat h((UNSIGNED_CHARACTER)MarketDataHandlerMap::getInstance().get(dataFeedVenueId)->isConnected(),
              (UNSIGNED_CHARACTER)OrderHandlerMap::getInstance().get(orderVenueId)->isConnected());
          char buf[MAX_BUF];
          size_t bytes = h.Serialize(buf);
          write(buf, bytes);
          break;
        }
      case CommandCategory_ORDERRESYNC_REQ:
        {
          LOG(INFO) << "[SERVER] Received Order Resync Request";
          OrderResync orsync(buffer + offset, true);
          char buf[ServerConstant::MAX_BUF_SIZE];
          if (orsync.GetLastIndex() < OrderStore::getInstance().getOrdersCount())
          {
            Order ord = OrderStore::getInstance().getOrderAt(orsync.GetLastIndex());
            ord.dumpOrder();
            orsync.SetOrderStr(ord.toString().c_str());
            size_t bytes = orsync.Serialize(buf, false);
            write(buf, bytes);
          }
          else
          {
            char buf[MAX_BUF];
            int bytes = 0;
            UNSIGNED_SHORT tmpShort = 0;
            UNSIGNED_CHARACTER tmpChar;

            bytes = sizeof(UNSIGNED_SHORT);  // Leave 2 bytes for packet size
            SERIALIZE_8(tmpChar, (UNSIGNED_CHARACTER)(CommandCategory_ORDERRESYNC_DONE), buf, bytes);  //Command Category

            UNSIGNED_SHORT dummyBytes = 0;
            SERIALIZE_16(tmpShort, (UNSIGNED_SHORT)(bytes - sizeof(UNSIGNED_SHORT)), buf, dummyBytes);
            write(buf, bytes);
            LOG(INFO) << "[SERVER] Order Resync Complete";
          }
          break;
        }
      case CommandCategory_ALERT:
        {
          LOG(INFO) << "[SERVER] Resync Alerts";
          AlertResync arsync(buffer + offset);
          std::lock_guard<std::mutex> lock(AlertMessageStore::getInstance().getMutex());
          if (arsync.GetLastIndex() < AlertMessageStore::getInstance().getSink().size())
          {
            Alerts a = AlertMessageStore::getInstance().getSink().at(arsync.GetLastIndex());
            char buf[MAX_BUF];
            size_t bytes = a.Serialize(buf);
            write(buf, bytes);
          }
          else
          {
            char buf[MAX_BUF];
            int bytes = 0;
            UNSIGNED_SHORT tmpShort = 0;
            UNSIGNED_CHARACTER tmpChar;

            bytes = sizeof(UNSIGNED_SHORT);  // Leave 2 bytes for packet size
            SERIALIZE_8(tmpChar, (UNSIGNED_CHARACTER)(CommandCategory_RESYNC_ALERTS_DONE), buf, bytes);  //Command Category

            UNSIGNED_SHORT dummyBytes = 0;
            SERIALIZE_16(tmpShort, (UNSIGNED_SHORT)(bytes - sizeof(UNSIGNED_SHORT)), buf, dummyBytes);
            write(buf, bytes);
            LOG(INFO) << "[SERVER] Alerts Resync Complete";
          }
          break;
        }

      default:
        LOG(WARNING) << "[SERVER] Unhandled Command [" << (UNSIGNED_INTEGER)commandCategory;
        break;
    }
  }

  void ClientConnection::handleRead(int fd, short ev)
  {
    char buf[ServerConstant::MAX_BUF_SIZE];
    int len;

#ifdef _WIN32
    len = recv(fd, buf, MAX_BUF_SIZE, 0);
#else
    len = read(fd, buf, ServerConstant::MAX_BUF_SIZE);
#endif
    if (len == 0)
    {
      /* Client disconnected, remove the read event and the
       * delete the client structure. */
      LOG(INFO)<<"[Server] Client[" << _clientName<<"] disconnected.";
      {
        std::lock_guard<std::mutex> l(_socketWriteMutex);
#ifdef _WIN32
        CloseHandle((HANDLE)fd);
#else
        close(fd);
#endif
      }
      {
        std::lock_guard<std::mutex> l(ConnectionManager::getInstance().getMutex());
        ConnectionManager::getInstance().removeConnectionForUser(_clientName);
      }
      //event_del(_evRead);
      //event_del(_evConnectionHealthCheck);
      this->setConnectionStatus(false);
      return;
    }
    else if (len < 0)
    {
      /* Some other error occurred, close the socket, remove
       * the event and free the client structure. */
      LOG(INFO) << "[Server] Socket failure, disconnecting client: "<< strerror(errno);
      {
        std::lock_guard<std::mutex> l(_socketWriteMutex);
#ifdef _WIN32
        CloseHandle((HANDLE)fd);
#else
        close(fd);
#endif
      }
      {
        std::lock_guard<std::mutex> l(ConnectionManager::getInstance().getMutex());
        ConnectionManager::getInstance().removeConnectionForUser(_clientName);
      }
      //event_del(_evRead);
      //event_del(_evConnectionHealthCheck);
      this->setConnectionStatus(false);
      return;
    }
    else
    {
      handleReadHeader(buf, len);
    }
  }

  void ClientConnection::handleHealthCheck(short ev)
  {
    time_t currentTime = time(0);
    if ((currentTime-_lastHearbeatTime)>(2*_heartbeatTimeout))
    {
      /* Client disconnected, remove the read event and the
       * delete the client structure. */
      {
        std::lock_guard<std::mutex> l(_socketWriteMutex);
#ifdef _WIN32
        CloseHandle((HANDLE)_socketFd);
#else
        close(_socketFd);
#endif
      }
      LOG(INFO)<<"Last heartbeat received at ["<< _lastHearbeatTime << "] current time ["
        << currentTime<<"] . Timeout Client [" << _clientName <<"] disconnected";
      {
        std::lock_guard<std::mutex> l(ConnectionManager::getInstance().getMutex());
        ConnectionManager::getInstance().removeConnectionForUser(_clientName);
      }
      //event_del(_evRead);
      //event_del(_evConnectionHealthCheck);
      this->setConnectionStatus(false);
      return;
    }
  }

  void ClientConnection::pushScripMasterData()
  {
    if(SymbolDictionary::getInstance().getSize() > 0)
    {
      LOG(INFO) << "[Server] SIZE [" <<   SymbolDictionary::getInstance().getDictonaryMap().size() << "]  ["
        << SymbolDictionary::getInstance().getSize() << "]";

      for(auto itr = SymbolDictionary::getInstance().getDictonaryMap().begin();
          itr != SymbolDictionary::getInstance().getDictonaryMap().end();
          itr++)
      {
        ScripMasterDataRequest scripMasterDataRequest;
        char responseBuffer[ServerConstant::MAX_BUF_SIZE];
        LOG(INFO) << "[Server] Symbol [" << itr->second.getSymbolName() <<"] Tick Size ["
          << itr->second.getFXCMSymPointSize() << "] Lot Size ["
          << itr->second.getLotSize() << "] Precision ["
          << itr->second.getFXCMSymPrecision()-'0' <<"]";
        scripMasterDataRequest.SetSymbol(itr->second.getSymbolName().c_str());
        scripMasterDataRequest.SetTickSize(itr->second.getFXCMSymPointSize());
        scripMasterDataRequest.SetLotSize(itr->second.getLotSize());
        scripMasterDataRequest.SetPrecission(itr->second.getFXCMSymPrecision() - '0');

        int bytes = scripMasterDataRequest.Serialize(responseBuffer);
        write(responseBuffer, bytes);
      }

      LOG(INFO) << "[SERVER] Dico transfer Complete....";
      LOG(INFO) << "[SERVER] Sending command to strategy for resync....";
    }

    char buf[MAX_BUF];
    int bytes = 0;
    UNSIGNED_SHORT tmpShort = 0;
    UNSIGNED_CHARACTER tmpChar;

    bytes = sizeof(UNSIGNED_SHORT);  // Leave 2 bytes for packet size
    SERIALIZE_8(tmpChar, (UNSIGNED_CHARACTER)(CommandCategory_SEND_SCRIP_MASTER_DATA_DONE), buf, bytes);  //Command Category

    UNSIGNED_SHORT dummyBytes = 0;
    SERIALIZE_16(tmpShort, (UNSIGNED_SHORT)(bytes - sizeof(UNSIGNED_SHORT)), buf, dummyBytes);
    write(buf, bytes);
    LOG(INFO) << "[SERVER] Script data Complete";
  }
}
