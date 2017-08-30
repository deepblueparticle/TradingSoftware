/*
 * =====================================================================================
 *
 *       Filename:  FixOPHandler.H
 *
 *    Description:
 *
 *        Created:  09/19/2016 06:53:14 AM
 *       Compiler:  g++
 *
 *         Author:  Soumya Prasad Ukil
 *        Company:  AlgoEngine
 *
 * =====================================================================================
 */

#ifndef FixOPHandler_H
#define FixOPHandler_H

#include <shared/iophandler.h>
#include <quickfix/Application.h>
#include <quickfix/MessageCracker.h>
#include <quickfix/fix44/OrderCancelReject.h>
#include <quickfix/SessionID.h>
#include <quickfix/ThreadedSocketInitiator.h>
#include <quickfix/FileStore.h>

namespace Algo
{
  class FixOPHandler : public FIX::Application, public FIX::MessageCracker, public IOpHandler
  {
    public:
      /// Get singleton instance
      static FixOPHandler& getInstance() {
        static FixOPHandler instance;
        return instance;
      }
      void start(std::string & fixConfigFile, int enableDebugLog = 0);
      void stop();
      /*!
       * \brief It checks if connection is still alive or not
       */
      bool isConnected() override;
      void setStartClOrderId(long id);
      static std::string getNextId();
      bool sendBracketOrder(const std::string& symbol,
                            double primaryEntryPrice,
                            double secondaryLimitPrice,
                            double secondaryStopPrice,
                            double secondaryStopLimitPrice,
                            double pointC,
                            int quantity,
                            OrderMode side,
                            double trailing,
                            UNSIGNED_LONG strategyId,
                            std::string & firstLegClOrderId,
                            std::string & secondLegLimitClOrderId,
                            std::string & secondLegStopClOrderId,
                            long internalId,
                            AlgoType algoType,
                            bool isIAC,
                            const std::string& upname,
                            const std::string& unique) override;

      bool modifyOrder(const std::string& originalOrderId,
          std::string& newOrderId,
          double price,
          double qty, long internalId) override;

      /*!
       * \brief return       the price for the SecondaryLimit order when performing t-exit
       *                     for FXCM, we add marketPrice and tickSize together
       * \param marketPrice  the market price
       * \param tickSize     positive tick size if it's buy strategy,
       *                     negative tick size if it's sell strategy
       */
      double modifiedSecondaryLimitPrice(double marketPrice, double tickSize) override;

    private:
      FixOPHandler();
      void onCreate(const FIX::SessionID &sessionID);
      void onLogon(const FIX::SessionID &sessionID);
      void onLogout(const FIX::SessionID &sessionID);

      /**
       * Message from Client to FIXAdapter.
       *
       * @return void
       */
      void toAdmin(FIX::Message&, const FIX::SessionID &sessionId);
      void setHeader(FIX::Message&, const FIX::SessionID &sessionId);
      /**
       * Heartbeat/logon messages received from FIXAdapter.
       *
       * @return void
       */
      void fromAdmin (const FIX::Message & message,
          const FIX::SessionID &) throw (FIX::FieldNotFound,
            FIX::IncorrectDataFormat,
            FIX::IncorrectTagValue,
            FIX::RejectLogon);
      /**
       * Trade message to FIXAdapter from Client.
       *
       * @return void
       */
      void toApp(FIX::Message &message,const FIX::SessionID&) throw (FIX::DoNotSend);
      void fromApp (const FIX::Message & message,
          const FIX::SessionID & sessionID) throw (FIX::FieldNotFound,
            FIX:: IncorrectDataFormat,
            FIX::IncorrectTagValue,
            FIX::UnsupportedMessageType);

      void onMessage(const FIX44::Logout &message, const FIX::SessionID&);
      void onMessage(const FIX44::Logon &message, const FIX::SessionID&);
      void onMessage(const FIX44::TradingSessionStatus &message,const FIX::SessionID&);
      void onMessage(const FIX44::OrderCancelReject &message, const FIX::SessionID&);
      void onMessage(const FIX44::BusinessMessageReject &message,const FIX::SessionID&);
      void onMessage(const FIX44::ExecutionReport &message, const FIX::SessionID&);
      void setSessionInf(const FIX::SessionID &sessionId);

    private:
      FIX::ThreadedSocketInitiator *_initiator;
      FIX::SessionSettings         *_settings;
      FIX::FileStoreFactory        *_storeFactory;
      FIX::ScreenLogFactory        *_logFactory;

      static long                  _clOrdId;
      static FIX::SessionID        _sessioId;
      static int                   _enableDebugLog;
      bool                         _isConnected;
  };
}

#endif
