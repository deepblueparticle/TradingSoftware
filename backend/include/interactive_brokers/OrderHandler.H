#ifndef Ib_OrderHandler_H
#define Ib_OrderHandler_H

#include <shared/iophandler.h>
#include <shared/order.h>
#include <ib_api/client/Contract.h>
#include <ib_api/client/Execution.h>
#include <interactive_brokers/ConnectionHandler.H>


namespace Algo
{
namespace Ib
{

/*! \class    OrderHandler
  * \brief    Handler for orders.
  */
class OrderHandler : public ConnectionHandler, public IOpHandler
{
  public:
    /// Get singleton instance
    static OrderHandler& getInstance() {
      static OrderHandler instance;
      return instance;
    }

    virtual ~OrderHandler();

    /*!
     * \brief Connect to TWS or IB Gateway API connection.
     *        Added additional operations for Order connection.
     *
     * \param host IP or hostname of the computer running IB Gateway instance.
     * \param port Port of the computer running IB Gateway instance.
     *
     * \return True if connected successfully, otherwise false.
     */
    virtual bool connect(const std::string& host, const int port) override;

    /*!
     * \brief It checks if connection is still alive or not
     */
    bool isConnected() override;

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
                          std::string & primaryEntryClOrderId,
                          std::string & secondaryLimitClOrderId,
                          std::string & secondaryStopClOrderId,
                          long internalId,
                          AlgoType algoType,
                          bool isIAC,
                          const std::string& upname,
                          const std::string& unique) override;

    bool modifyOrder(const std::string& originalOrderId,
                     std::string& newOrderId,
                     double price,
                     double qty,
                     long internalId) override;

    /*!
     * \brief return       the price for the SecondaryLimit order when performing t-exit
     *                     for IB, we just return marketPrice
     * \param marketPrice  the market price
     * \param tickSize     positive tick size if it's buy strategy,
     *                     negative tick size if it's sell strategy
     */
    double modifiedSecondaryLimitPrice(double marketPrice, double tickSize) override;


    /*!
     * \brief Call back to get next valid id to send new order.
     *
     * \param orderId The new id
     *
     */
    virtual void nextValidId(long orderId) override;

    /*!
     * \brief Call back to get status update of order.
     *
     * \param status: PendingSubmit - indicates that you have transmitted the order,
     *                  but have not yet received confirmation that it has been accepted by the order destination.
     *                  NOTE: This order status is not sent by TWS and should be explicitly set by
     *                  the API developer when an order is submitted.
     *                PendingCancel - indicates that you have sent a request to cancel the order
     *                  but have not yet received cancel confirmation from the order destination. At this point,
     *                  your order is not confirmed canceled. You may still receive an execution while
     *                  your cancellation request is pending. NOTE: This order status is not sent by TWS and
     *                  should be explicitly set by the API developer when an order is canceled.
     *                PreSubmitted - indicates that a simulated order type has been accepted by
     *                  the IB system and that this order has yet to be elected. The order is held in
     *                  the IB system until the election criteria are met. At that time the order is
     *                  transmitted to the order destination as specified .
     *                Submitted - indicates that your order has been accepted at
     *                  the order destination and is working.
     *                ApiCanceled - after an order has been submitted and before
     *                  it has been acknowledged, an API client client can request its cancelation,
     *                  producing this state.
     *                Cancelled - indicates that the balance of your order has been
     *                  confirmed canceled by the IB system. This could occur unexpectedly
     *                  when IB or the destination has rejected your order.
     *                Filled - indicates that the order has been completely filled.
     *                  Inactive - indicates that the order has been accepted by
     *                  the system (simulated orders) or an exchange (native orders) but that
     *                  currently the order is inactive due to system, exchange or other issues.
     *
     */
    virtual void orderStatus(OrderId orderId, const std::string& status,
                             double filled, double remaining,
                             double avgFillPrice, int permId, int parentId,
                             double lastFillPrice, int clientId,
                             const std::string& whyHeld) override;
    /*!
     * \brief Call back to get fill of order.
     *
     * \param reqId:      the request's identifier, in Paper account always -1, not sure about real account
     * \param execution:  Execution (int orderId, int clientId, String execId, String time,
     *                               String acctNumber, String exchange, String side, double shares,
     *                               double price, int permId, int liquidation, double cumQty, double avgPrice,
     *                               String orderRef, String evRule, double evMultiplier, String modelCode)
     *
     */

    virtual void execDetails(int reqId, const ::Contract& contract, const ::Execution& execution) override;

    /*!
     * \brief Create Order from IB Order.
     *
     * \param ibOrder Order from IB Api.
     * \param [out] outAgloOrder Output Order.
     *
     * \return True if successfully converted, otherwise false.
     */
    static bool ibOrderToAlgoOrder(const ::Order& ibOrder,
                                   Algo::Order& outAgloOrder);

    /*!
     * \brief Create IB API Order from our Algo Order.
     *
     * \param agloOrder Output Order.
     * \param [out] outIbOrder Order from IB Api.
     *
     * \return True if successfully converted, otherwise false.
     */
    static bool algoOrderToIbOrder(const Algo::Order& agloOrder,
                                   ::Order& outIbOrder);

  private:
    OrderHandler();
    OrderHandler(const OrderHandler&);

    virtual void error(const int id, const int errorCode,
                       const std::string errorStr) override;

    /*!
     * \brief  Send bracket order with params which are objects inside of IB API
     *
     * \param[in]  side            true for buy, false for sell
     * \param[out] primaryEntry    The generated Primary Entry
     * \param[out] secondaryLimit  The generated Secondary Limit
     * \param[out] secondaryStop   The generated Secondary Stop
     */
    void createBracketOrderWithTrailing(
        int primaryEntryOrderId, ::Order& primaryEntry, ::Order& secondaryLimit,
        ::Order& secondaryStop, bool side, double quantity, double limitPrice,
        double secondaryLimitPrice, double secondaryStopPrice,
        double secondaryStopLimitPrice);

    /*!
     * \brief  Convert bracket orders to string
     *
     */
    std::string bracketOrderAsString(
        const ::Order& primaryEntry, const ::Order& secondaryLimit,
        const ::Order& secondaryStop, const ::Contract& symbol);

    /*!
     * \brief  Convert primary entry order to string
     *
     */
    std::string primaryEntryOrderAsString(const ::Order& primaryEntry);

    /*!
     * \brief  Convert secondary limit order to string
     *
     */
    std::string secondaryLimitOrderAsString(const ::Order& secondaryLimit);

    /*!
     * \brief  Convert secondary stop order to string
     *
     */
    std::string secondaryStopOrderOrderAsString(const ::Order& secondaryStop);

    /*!
     * \brief Implementation of send bracket order with IB API
     *
     * \param[in]  secondaryStopTrailingAmount   trailing
     * \param[out] primaryEntry                  The generated Primary Entry
     * \param[out] secondaryLimit                The generated Secondary Limit
     * \param[out] secondaryStop                 The generated Secondary Stop
     * \return                                   true if send succeed, otherwise false.
     */
    bool sendBracketOrder(
        const std::string& symbol, double primayrEntryPrice,
        double secondaryLimitPrice,
        double secondaryStopPrice,
        double secondaryStopLimitPrice,
        int quantity, OrderMode side,
        ::Order& primaryEntry,
        ::Order& secondaryLimit,
        ::Order& secondaryStop);

    /*!
    * \brief Modify existing order price and quantity.
    *
    * \param order The original order to modify.
    * \param newPice New price for the order.
    * \param newQuantity New quantity for the order.
    *
    * \return True if successfully converted, otherwise false.
    */
    bool modifyOrder(Algo::Order *order, double newPice, double newQuantity);

    long _orderId;
};

} // namespace Ib
} // namespace Algo

#endif
