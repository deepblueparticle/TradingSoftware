#ifndef Ib_ConnectionHandler_H
#define Ib_ConnectionHandler_H

#include <ib_api/client/EReaderOSSignal.h>
#include <ib_api/client/EWrapper.h>

// Forward declaration
class EClientSocket;
class EReader;

namespace Algo
{
namespace Ib
{

/*! \class    ConnectionHandler
  * \brief    Base class to handle connecton to IB Gateway or TWS.
  */
class ConnectionHandler : public EWrapper
{
  public:
    ConnectionHandler();
    virtual ~ConnectionHandler();

    /*!
     * \brief Connect to TWS or IB Gateway API connection.
     *
     * \param host IP or hostname of the computer running IB Gateway instance.
     * \param port Port of the computer running IB Gateway instance.
     *
     * \return True if connected successfully, otherwise false.
     */
    virtual bool connect(const std::string& host, const int port);

    /*
     * \brief Disconnect from TWS or IB Gateway API connection.
     */
    void disconnect() const;

    /*
     * \brief Check whether API connection present.
     * \return True if connected, otherwise false.
     */
    bool isConnected() const;

    /*!
     * \brief This is necessary to call in infinite loop to get callbacks from
     *        IB API.
     */
    void processMessages();

  protected:
    // EWrapper provides these pure virtual methods, but not all
    // of them needed even in our child classes, so here default empty
    // implementations
    virtual void error(const int id, const int errorCode,
                       const std::string errorStr) override;

    virtual void tickPrice(TickerId tickerId, TickType field, double price,
                           int canAutoExecute) override {}
    virtual void tickSize(TickerId tickerId, TickType field,
                          int size) override {}
    virtual void tickOptionComputation(TickerId tickerId, TickType tickType,
                                       double impliedVol, double delta,
                                       double optPrice, double pvDividend,
                                       double gamma, double vega, double theta,
                                       double undPrice) override {}
    virtual void tickGeneric(TickerId tickerId, TickType tickType,
                             double value) override {}
    virtual void tickString(TickerId tickerId, TickType tickType,
                            const std::string& value) override {}
    virtual void tickEFP(TickerId tickerId, TickType tickType,
                         double basisPoints,
                         const std::string& formattedBasisPoints,
                         double totalDividends, int holdDays,
                         const std::string& futureLastTradeDate,
                         double dividendImpact,
                         double dividendsToLastTradeDate) override {}
    virtual void orderStatus(OrderId orderId, const std::string& status,
                             double filled, double remaining,
                             double avgFillPrice, int permId, int parentId,
                             double lastFillPrice, int clientId,
                             const std::string& whyHeld) override {}
    virtual void openOrder(OrderId orderId, const Contract&, const ::Order&,
                           const OrderState&) override {}
    virtual void openOrderEnd() override {}
    virtual void winError(const std::string& str, int lastError) override {}
    virtual void connectionClosed() override {}
    virtual void updateAccountValue(const std::string& key,
                                    const std::string& val,
                                    const std::string& currency,
                                    const std::string& accountName) override {}
    virtual void updatePortfolio(const Contract& contract, double position,
                                 double marketPrice, double marketValue,
                                 double averageCost, double unrealizedPNL,
                                 double realizedPNL,
                                 const std::string& accountName) override {}
    virtual void updateAccountTime(const std::string& timeStamp) override {}
    virtual void accountDownloadEnd(const std::string& accountName) override {}
    virtual void nextValidId(OrderId orderId) override {}
    virtual void contractDetails(int reqId,
                         const ContractDetails& contractDetails) override {}
    virtual void bondContractDetails(int reqId,
                             const ContractDetails& contractDetails) override {}
    virtual void contractDetailsEnd(int reqId) override {}
    virtual void execDetails(int reqId, const Contract& contract,
                             const Execution& execution) override {}
    virtual void execDetailsEnd(int reqId) override {}
    virtual void updateMktDepth(TickerId id, int position, int operation,
                                int side, double price, int size) override {}
    virtual void updateMktDepthL2(TickerId id, int position,
                                  std::string marketMaker, int operation,
                                  int side, double price, int size) override {}
    virtual void updateNewsBulletin(int msgId, int msgType,
                                    const std::string& newsMessage,
                                    const std::string& originExch) override {}
    virtual void managedAccounts(const std::string& accountsList) override {}
    virtual void receiveFA(faDataType pFaDataType,
                           const std::string& cxml) override {}
    virtual void historicalData(TickerId reqId, const std::string& date,
                                double open, double high, double low,
                                double close, int volume, int barCount,
                                double WAP, int hasGaps) override {}
    virtual void scannerParameters(const std::string& xml) override {}
    virtual void scannerData(int reqId, int rank,
                             const ContractDetails& contractDetails,
                             const std::string& distance,
                             const std::string& benchmark,
                             const std::string& projection,
                             const std::string& legsStr) override {}
    virtual void scannerDataEnd(int reqId) override {}
    virtual void realtimeBar(TickerId reqId, long time, double open,
                             double high, double low, double close, long volume,
                             double wap, int count) override {}
    virtual void currentTime(long time) override {}
    virtual void fundamentalData(TickerId reqId,
                                 const std::string& data) override {}
    virtual void deltaNeutralValidation(int reqId,
                                        const UnderComp& underComp) override {}
    virtual void tickSnapshotEnd(int reqId) override {}
    virtual void marketDataType(TickerId reqId, int marketDataType) override {}
    virtual void commissionReport(
        const CommissionReport& commissionReport) override {}
    virtual void position(const std::string& account, const Contract& contract,
                          double position, double avgCost) override {}
    virtual void positionEnd() override {}
    virtual void accountSummary(int reqId, const std::string& account,
                                const std::string& tag,
                                const std::string& value,
                                const std::string& curency) override {}
    virtual void accountSummaryEnd(int reqId) override {}
    virtual void verifyMessageAPI(const std::string& apiData) override {}
    virtual void verifyCompleted(bool isSuccessful,
                                 const std::string& errorText) override {}
    virtual void displayGroupList(int reqId,
                                  const std::string& groups) override {}
    virtual void displayGroupUpdated(int reqId,
                                     const std::string& contractInfo) override {
    }
    virtual void verifyAndAuthMessageAPI(
        const std::string& apiData, const std::string& xyzChallange) override {}
    virtual void verifyAndAuthCompleted(bool isSuccessful,
                                        const std::string& errorText) override {
    }
    virtual void connectAck() override {}
    virtual void positionMulti(int reqId, const std::string& account,
                               const std::string& modelCode,
                               const Contract& contract, double pos,
                               double avgCost) override {}
    virtual void positionMultiEnd(int reqId) override {}
    virtual void accountUpdateMulti(int reqId, const std::string& account,
                                    const std::string& modelCode,
                                    const std::string& key,
                                    const std::string& value,
                                    const std::string& currency) override {}
    virtual void accountUpdateMultiEnd(int reqId) override {}
    virtual void securityDefinitionOptionalParameter(
        int reqId, const std::string& exchange, int underlyingConId,
        const std::string& tradingClass, const std::string& multiplier,
        std::set<std::string> expirations, std::set<double> strikes) override {}
    virtual void securityDefinitionOptionalParameterEnd(int reqId) override {}
    virtual void softDollarTiers(
        int reqId, const std::vector<SoftDollarTier>& tiers) override {}

    EClientSocket* const _client; /*!< socket connection to IB Gateway */
    EReaderOSSignal _osSignal;
    EReader* _reader; /*!< Captures incoming messages to the API client and
                           places them into a queue. */
};

} // namespace Ib
} // namespace Algo

#endif
