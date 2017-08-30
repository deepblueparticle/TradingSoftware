/*
 * =====================================================================================
 *
 *       Filename:  FixMessageHandler.C
 *
 *    Description:  
 *
 *        Created:  09/24/2016 03:13:59 AM
 *       Compiler:  g++
 *
 *         Author:  Soumya Prasad Ukil
 *        Company:  AlgoEngine
 *
 * =====================================================================================
 */

#include <shared/easylogging++.h>
#include <fix/FixMessageHandler.H>
#include <fix/FixMessageReader.H>
#include <symbol/SymbolDictionary.H>
#include <quote/QuoteBook.H>
#include <quickfix/fix44/SecurityListRequest.h>
#include <quickfix/fix44/TradingSessionStatusRequest.h>
#include <quickfix/fix44/BusinessMessageReject.h>
#include <quickfix/fix44/MarketDataRequest.h>
#include <quickfix/fix44/TradingSessionStatus.h>
#include <quickfix/fix44/SecurityList.h>
#include <quickfix/fix44/Logout.h>
#include <quickfix/fix44/Logon.h>
#include <connection/ConnectionStatus.H>

namespace Algo
{
  FixMessageHandler::FixMessageHandler ()
  : _settings( nullptr ),
    _storeFactory( nullptr ),
    _logFactory( nullptr ),
    _initiator( nullptr ),
    _id(11000)
  {
  }

  FixMessageHandler::~FixMessageHandler ()
  {
    if( _settings )
    {
      delete _settings;
      _settings = nullptr;
    }
    if( _storeFactory )
    {
      delete _storeFactory;
      _storeFactory = nullptr;
    }
    if( _logFactory )
    {
      delete _logFactory;
      _logFactory = nullptr;
    }
    if( _initiator )
    {
      delete _initiator;
      _initiator = nullptr;
    }
  }

  void FixMessageHandler::start(std::string& fixConfigFile , int enableINFOLog)
  {
    _enableDebugLog = enableINFOLog;
    _settings = new FIX::SessionSettings( fixConfigFile );
    _storeFactory = new FIX::FileStoreFactory(*_settings);
    _logFactory = new FIX::ScreenLogFactory(*_settings);

    _initiator =
      new FIX::ThreadedSocketInitiator( *this,
          *_storeFactory,
          *_settings,
          *_logFactory );
    _initiator->start();
  }

  void FixMessageHandler::stop()
  {
    _initiator->stop();
  }

  long long int FixMessageHandler::getNextId()
  {
    return ++_id;
  }

  void FixMessageHandler::onCreate(const FIX::SessionID &sessionID)
  {
  }

  void FixMessageHandler::onLogon( const FIX::SessionID& sessionID )
  {
    LOG(INFO) <<"[FXCM FEED] Logon Received";
    ConnectionStatus::getInstance().setFeedConnected(true);
    setSessionInf(sessionID);
    securityListRequest(sessionID);
  }

  bool FixMessageHandler::isConnected() 
  {
    return _isConnected;
  }

  void FixMessageHandler::securityListRequest(const FIX::SessionID& sessionID)
  {
    FIX44::SecurityListRequest securityListReq;
    securityListReq.set(FIX::SecurityReqID(std::to_string(getNextId())));
    securityListReq.set(FIX::SecurityListRequestType(4)); //4 = ALL.

    if(_enableDebugLog)
    {
      FixMessageReader::deserialize( "OUTBOUND [FXCM FEED]", securityListReq.toString() ) ;
    }
    LOG(INFO) << "[FXCM FEED] Sending symbol list req " << securityListReq.toString();
    FIX::Session::sendToTarget( securityListReq, sessionID);
  }

  void FixMessageHandler::onLogout(const FIX::SessionID &sessionID)
  {
  }

  void FixMessageHandler::toAdmin(FIX::Message& message, const FIX::SessionID& sessionId)
  {
    FIX::MsgType msgType;
    message.getHeader().getField(msgType);

    setHeader(message, sessionId);
    if(((msgType.getValue())) == (FIX::MsgType_Logon))
    {
      const FIX::Dictionary& sessionSettings = _settings->get(sessionId);
      if (sessionSettings.has("Username"))
      {
        message.setField(FIX::Username(sessionSettings.getString("Username")));
      }
      if (sessionSettings.has("Password"))
      {
        message.setField(FIX::Password(sessionSettings.getString("Password")));
      }

      message.setField  ( FIX::EncryptMethod ( 0 ) );
      message.getHeader().setField ( FIX::MsgSeqNum (1) );
      message.setField ( FIX::ResetSeqNumFlag (FIX::ResetSeqNumFlag_YES));
      LOG(INFO)  <<  "[FXCM FEED] Sending logon message " << message.toString();
    }
  }

  void FixMessageHandler::setHeader(FIX::Message& message, const FIX::SessionID &sessionId)
  {
    const FIX::Dictionary& sessionSettings = _settings->get( sessionId );
    message.getHeader().setField(
        FIX::SenderCompID(sessionSettings.getString("SenderCompID")));

    message.getHeader().setField(
        FIX::TargetCompID(sessionSettings.getString("TargetCompID")));

    message.getHeader().setField(
        FIX::TargetSubID(sessionSettings.getString("TargetSubID")));
  }

  void FixMessageHandler::fromAdmin(const FIX::Message &message, const FIX::SessionID&) throw(  
      FIX::FieldNotFound,
      FIX::IncorrectDataFormat,
      FIX::IncorrectTagValue,
      FIX::RejectLogon )
  {
    //LOG(DEBUG) << "[FXCM FEED] Message Received " << message.toString();
    if(_enableDebugLog)
    {
      FixMessageReader::deserialize("INBOUND [FXCM FEED]", message.toString());
    }
  }

  void FixMessageHandler::toApp( FIX::Message& message,
      const FIX::SessionID&
      ) throw( FIX::DoNotSend )
  {
    if(_enableDebugLog)
    {
      FixMessageReader::deserialize("INBOUND [FXCM FEED]", message.toString());
    }
    //LOG(DEBUG) << "[FXCM FEED] Message Received " << message.toString();
  }

  void FixMessageHandler::fromApp(const FIX::Message& message,const FIX::SessionID& sessionID) throw( 
      FIX::FieldNotFound,
      FIX::IncorrectDataFormat,
      FIX::IncorrectTagValue,
      FIX::UnsupportedMessageType )
  {
    try
    {
      crack( message, sessionID );
    }
    catch(std::exception &e)
    {
      LOG(WARNING) << "[FXCM FEED] Crack Exception " << e.what();
    }
    catch(...)
    {
      LOG(WARNING) << "[FXCM FEED] Unknown Crack Exception " ;
    }
  }

  void FixMessageHandler::tradingSessionStatusReq(const FIX::SessionID &sessionID)
  {
    LOG(INFO) << "[FXCM FEED] Dico Size = [" <<    SymbolDictionary::getInstance().getSize() <<"]";

    FIX44::TradingSessionStatusRequest request;
    request.set(FIX::TradSesReqID(std::to_string(getNextId())));
    request.set(FIX::SubscriptionRequestType(
          FIX::SubscriptionRequestType_SNAPSHOT_PLUS_UPDATES));

    if(_enableDebugLog)
    {
      FixMessageReader::deserialize("OUTBOUND [FXCM FEED]", request.toString());
    }
    LOG(INFO) << "[FXCM FEED] Sending Message "<< request.toString();
    FIX::Session::sendToTarget(request, sessionID);
  }

  void FixMessageHandler::onMessage(const FIX44::Logout &message, const FIX::SessionID &sessionID)
  {
    if(_enableDebugLog)
    {
      FixMessageReader::deserialize("INBOUND [FXCM FEED]", message.toString());
    }
    LOG(INFO) << "[FXCM FEED] Message Received "<< message.toString();
    LOG(INFO) << "[FXCM FEED] Logout received------------";
    _isConnected = false;
  }

  void FixMessageHandler::onMessage(const FIX44::SecurityListRequest&, const FIX::SessionID&)
  {
    LOG(INFO) << "[FXCM FEED] SecurityListRequest Received";
  }

  void FixMessageHandler::onMessage(const FIX44::Logon &message, const FIX::SessionID &sessionID)
  {
    if(_enableDebugLog)
    {
      FixMessageReader::deserialize("INBOUND [FXCM FEED]", message.toString());
    }
    LOG(INFO) << "[FXCM FEED] Message Received "<< message.toString();
    LOG(INFO) << "[FXCM FEED] Logon received------------";
    _isConnected = true;
  }

  void FixMessageHandler::onMessage(const FIX44::TradingSessionStatus &tss, const FIX::SessionID &sessionID)
  {
    LOG(INFO) << "[FXCM FEED] Message Received " << tss.toString();
    if(_enableDebugLog)
    {
      FixMessageReader::deserialize("INBOUND [FXCM FEED]", tss.toString());
    }
    FIX44::MarketDataRequest request;
    request.setField(FIX::MDReqID(std::to_string(getNextId())));
    request.setField(FIX::SubscriptionRequestType(
          FIX::SubscriptionRequestType_SNAPSHOT_PLUS_UPDATES));
    request.setField(FIX::MarketDepth(0));

    request.setField(FIX::MDUpdateType(1) );

    FIX44::MarketDataRequest::NoMDEntryTypes entry_types;
    entry_types.setField(FIX::MDEntryType(FIX::MDEntryType_BID));
    request.addGroup(entry_types);
    entry_types.setField(FIX::MDEntryType(FIX::MDEntryType_OFFER));
    request.addGroup(entry_types);
    entry_types.setField(FIX::MDEntryType(FIX::MDEntryType_TRADING_SESSION_HIGH_PRICE));
    request.addGroup(entry_types);
    entry_types.setField(FIX::MDEntryType(FIX::MDEntryType_TRADING_SESSION_LOW_PRICE));
    request.addGroup(entry_types);

    int symbols_count = FIX::IntConvertor::convert(tss.getField(FIX::FIELD::NoRelatedSym));
    request.setField(FIX::NoRelatedSym(symbols_count));
    for(int i = 1; i <= symbols_count; i++){
      FIX44::SecurityList::NoRelatedSym symbols_group_SL;
      tss.getGroup(i,symbols_group_SL);
      std::string symbol = symbols_group_SL.getField(FIX::FIELD::Symbol);

      FIX44::MarketDataRequest::NoRelatedSym symbols_group_MDR;
      symbols_group_MDR.setField(FIX::Symbol(symbol));
      request.addGroup(symbols_group_MDR);
    }

    FIX::Session::sendToTarget(request, sessionID);
  }

  void FixMessageHandler::onMessage(const FIX44::BusinessMessageReject &message, const FIX::SessionID &sessionID)
  {
    if(_enableDebugLog)
    {
      FixMessageReader::deserialize("INBOUND [FXCM FEED]", message.toString());
    }
    LOG(WARNING) << "[FXCM FEED] BusinessMessageReject received " << message.toString();
  }

  void FixMessageHandler::onMessage(const FIX44::SecurityList &message, const FIX::SessionID &sessionID)
  {
    FIX::NoRelatedSym symbolCount;
    message.get(symbolCount);

    if(_enableDebugLog)
    {
      FixMessageReader::deserialize("INBOUND [FXCM FEED]", message.toString());
    }
    LOG(INFO) << "[FXCM FEED] Message Received " << message.toString();
    LOG(INFO) << "Total symbol counts in the symbol list [" << int(symbolCount) <<"]";
    for(auto i=1; i<=symbolCount; ++i )
    {
      FIX44::SecurityList::NoRelatedSym relatedSymbols;
      message.getGroup(i, relatedSymbols);
      FIX::Symbol symbol;
      relatedSymbols.get(symbol);

      FIX::Product pt;
      relatedSymbols.get(pt);
      if(pt == FIX::Product_CURRENCY)
      {
        SymbolStaticData sd;
        sd.setSymbolName(std::string(symbol));
        sd.setProductType(CURRENCY);

        FIX::Factor f;
        relatedSymbols.get(f);
        sd.setFactor(f);

        FIX::ContractMultiplier m;
        relatedSymbols.get(m);
        sd.setContractMultiplier(m);
        FIX::Currency curr;
        relatedSymbols.get(curr);
        sd.setCurrency(curr);

        FIX::RoundLot lots;
        relatedSymbols.get(lots);
        sd.setLotSize(lots);

        sd.setFXCMSymId(relatedSymbols.getField(9000));
        std::string str = relatedSymbols.getField(9001);
        int precission = std::atoi(str.c_str());
        sd.setFXCMSymPrecision(str[0]);
        str = relatedSymbols.getField(9002);
        sd.setFXCMSymPointSize(std::pow(10.0f, -precission));  //std::stof(str); //ticksize

        LOG(INFO) << "[FXCM FEED] m_FXCMSymPrecision ["<<sd.getFXCMSymPrecision() <<"] sd.m_FXCMSymPointSize ["
          << sd.getFXCMSymPointSize() <<"] precision ["<<precission<<"]";
        //            float        m_FXCMSymPointSize;
        //            uint32_t     m_FXCMMaxQuantity;
        //            uint32_t     m_FXCMMinQuantity;

        SymbolDictionary::getInstance().addSymbolData(sd);
        LOG(INFO) << "[FXCM FEED] Inserting Symbol ["<<std::string(symbol) <<"] to symbol set.";
      }
    }
    tradingSessionStatusReq( sessionID );
  }

  void FixMessageHandler::onMessage(const FIX44::MarketDataSnapshotFullRefresh &message, const FIX::SessionID&)
  {
    if(_enableDebugLog)
    {
      FixMessageReader::deserialize("INBOUND [FXCM FEED]", message.toString());
    }
    //LOG(INFO) << "[FXCM FEED] MarketDataSnapshotFullRefresh received " << message.toString();
    Quotes q;
    FIX::Symbol sym;
    message.get(sym);
    q.setSecurityId(sym);
    FIX::NoMDEntries  numEntries;
    message.get(numEntries);
    if(numEntries >= 2)
      for(auto i=1; i<=2; ++i )
      {
        FIX44::MarketDataSnapshotFullRefresh::NoMDEntries nmd;
        message.getGroup(i, nmd);
        FIX::MDEntryType mdt;
        nmd.get(mdt);
        if(mdt == '0') //Bid
        {
          FIX::MDEntryPx px;
          FIX::MDEntrySize  qt;
          nmd.get(px);
          nmd.get(qt);
          q.setBidPrice(px);
          q.setBidQty(qt);
        }
        else if(mdt == '1') //Ask
        {
          FIX::MDEntryPx px;
          FIX::MDEntrySize  qt;
          nmd.get(px);
          nmd.get(qt);
          q.setAskPrice( px);
          q.setAskQty( qt);
        }
        //        FIX::Currency cur;
        //        nmd.get(cur);
        //        q.setCurrency(cur);
      }
    QuoteBook::getInstance()->add(q.getSecurityId(), q);
    //QuoteBook::getInstance()->dump(sym);
  }

  void FixMessageHandler::onNew(const FIX44::ExecutionReport &message)
  {
  }

  void FixMessageHandler::onFilled(const FIX44::ExecutionReport &message)
  {
  }

  void FixMessageHandler::onMessage(const FIX44::ExecutionReport &message, const FIX::SessionID&)
  {
  }

  void FixMessageHandler::setSessionInf( const FIX::SessionID& sessionId)
  {
    //const FIX::Dictionary& sessionSettings = _settings->get(sessionId);
    //TODO :
  }
}
