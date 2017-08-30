/*
 * =====================================================================================
 *
 *       Filename:  FixMessageHandler.H
 *
 *    Description:
 *
 *        Created:  09/18/2016 12:05:41 PM
 *       Compiler:  g++
 *
 *         Author:  Soumya Prasad Ukil
 *        Company:  AlgoEngine
 *
 * =====================================================================================
 */

#ifndef FixMessageHandler_H
#define FixMessageHandler_H

#include <quickfix/FileStore.h>
#include <quickfix/ThreadedSocketInitiator.h>
#include <quickfix/Application.h>
#include <quickfix/MessageCracker.h>
#include <quickfix/fix44/MarketDataSnapshotFullRefresh.h>

#include <shared/imarket_data_handler.h>

namespace Algo
{
  class FixMessageHandler
    : public FIX::Application,
    public FIX::MessageCracker,
    public Algo::IMarketDataHandler
  {
    public:
      /// Get singleton instance
      static FixMessageHandler& getInstance() {
        static FixMessageHandler instance;
        return instance;
      }

      virtual ~FixMessageHandler ();
      virtual void start( std::string& fixConfigFile, int enableDebugLog=0 );
      virtual void stop();
      /*!
       * \brief It checks if connection is still alive or not
       */
      bool isConnected() override;

    protected:
      // TODO: implement later. See issue:
      // https://github.com/rslichte/AlgoEngine/issues/268
      virtual bool subscribeImplementation(const std::string& contract) {}
      virtual bool unsubscribeImplementation(const std::string& contract) {}

    private:
      FixMessageHandler();
      FixMessageHandler(const FixMessageHandler&);
      long long int getNextId();
      void onCreate(const FIX::SessionID& sessionID);
      void onLogon(const FIX::SessionID& sessionID);
      void securityListRequest(const FIX::SessionID& sessionID);
      void onLogout(const FIX::SessionID& sessionID);
      void toAdmin(FIX::Message&,const FIX::SessionID& sessionId );
      void setHeader(FIX::Message&, const FIX::SessionID& sessionId );
      /**
       * Heartbeat/logon messages received from FIXAdapter.
       *
       * @return void
       */
      void fromAdmin( const FIX::Message& message,const FIX::SessionID&) throw(FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::RejectLogon);
      /**
       * Trade message to FIXAdapter from Client.
       *
       * @return void
       */
      void toApp( FIX::Message& message,const FIX::SessionID&) throw(FIX::DoNotSend);
      /**
       * Trade message received from FIXAdapter.
       *
       * @return void
       */
      void fromApp( const FIX::Message& message,const FIX::SessionID& sessionID) throw( FIX::FieldNotFound,FIX::IncorrectDataFormat,FIX::IncorrectTagValue,FIX::UnsupportedMessageType);
      void tradingSessionStatusReq(const FIX::SessionID&);
      void onMessage(const FIX44::Logout& message,const FIX::SessionID&);
      void onMessage(const FIX44::SecurityListRequest&,const FIX::SessionID&);
      void onMessage(const FIX44::Logon& message,const FIX::SessionID&);
      void onMessage(const FIX44::TradingSessionStatus& message,const FIX::SessionID&);
      void onMessage(const FIX44::BusinessMessageReject &message,const FIX::SessionID&);
      void onMessage(const FIX44::SecurityList &message,const FIX::SessionID& sessionID);
      void onMessage(const FIX44::MarketDataSnapshotFullRefresh &message,const FIX::SessionID&);
      /**
       * @brief onNew
       * @param message
       */
      void onNew(const FIX44::ExecutionReport& message);

      /**
       * @brief onFilled
       * @param message
       */
      void onFilled(const FIX44::ExecutionReport& message);

      /**
       * @brief onRejected
       * @param message
       */
      void onRejected(const FIX44::ExecutionReport& message);

      /**
       * @brief onMessage
       * @param message
       */
      void onMessage(const FIX44::ExecutionReport& message,const FIX::SessionID&);
      void setSessionInf(const FIX::SessionID &sessionId);

      FIX::SessionSettings         *_settings;
      FIX::FileStoreFactory        *_storeFactory;
      FIX::ScreenLogFactory        *_logFactory;
      FIX::ThreadedSocketInitiator *_initiator;
      int                           _enableDebugLog;
      long long int                 _id;
      bool                          _isConnected;
  };
}

#endif
