#ifndef GENERICSTRATEGY_H
#define GENERICSTRATEGY_H

#include <functional>
#include <boost/serialization/access.hpp>
#include <boost/serialization/assume_abstract.hpp>

namespace Algo {

    class IDateTimeProvider;
    class IOpHandler;
    class IQuotesProvider;

    struct Elements;
    class Order;

    /*!
     *  \brief    Lamda function that handles queue of messages
     *            to send from backend to GUI
     */
    using PushToServerMsgQueue = std::function<void(const Elements&)>;

    /*! \class    GenericStrategy
     *  \brief    Generic class describing  strategy
     */
    class GenericStrategy {
      friend class boost::serialization::access;
    public:

        /*! A constructor
         *
         * \param serverMsgQueue Lamda function that handles queue of messages
         *                       to send from backend to GUI
         */
        explicit GenericStrategy(PushToServerMsgQueue serverMsgQueue = nullptr) :
               _serverMsgQueue(serverMsgQueue), _dateTimeProvider(nullptr), _opHandler(nullptr), _quotesProvider(nullptr),
               _isCompleted(false), _maxAttempts(0) { }

        virtual ~GenericStrategy() { }

        /*!
         * \brief Checks current state of the strategy, checks new quotes
         *        arrived from quotes feed and handles state-machine of the
         *        strategy.
         */
        virtual void processFeed() = 0;

        /*!
         * \brief Processes given order, updates stragey state according to
         *        received info, creates events and sends them into DB queue
         * \param order Market Order reperesentation
         */
        virtual void processOrder(const Order &order) = 0;

        void setDateTimeProvider(IDateTimeProvider *provider) { _dateTimeProvider = provider; }
        void setOpHandler(IOpHandler *opHandler) { _opHandler = opHandler; }
        void setQuotesProvider(IQuotesProvider *provider) { _quotesProvider = provider; }
        void setPushToServerMsgQueue(PushToServerMsgQueue msgQueue) { _serverMsgQueue = msgQueue; }

        bool isComplete() const { return _isCompleted; }
        void setMaxAttempts(int i) { _maxAttempts = i; }
        int getMaxAttempts() const { return _maxAttempts; }

        virtual void setCompleted() { _isCompleted = true; }

        /*!
         * \brief Stop strategy immediately
         */
        virtual void forceStrategyStop() = 0;
        virtual void sendCompleted() = 0;
        virtual void setAfterRestart() = 0;

    protected:
        IDateTimeProvider *_dateTimeProvider; /*!< Interface via which UTC time
                                                   be obtained */
        IOpHandler *_opHandler; /*!< Handler of operations with market */
        IQuotesProvider *_quotesProvider; /*!< Provider of quotes from market,
                                               which can be used to make
                                               decisions of strategy */
        PushToServerMsgQueue _serverMsgQueue; /*!< Lamda function that handles
                                                   queue of messages to send
                                                   from backend to GUI */

        bool _isCompleted; /*!< Whether strategy finished */
        bool _afterRestart; /*!< Whether strategy restarted */
        int _maxAttempts; /*!< Max number of attemts to send order when it
                               wasn't successfully sent and market still in
                               needed state */
    private:
        template<class Archive>
        void serialize(Archive &ar, const unsigned int version)
        {
          ar & _isCompleted;
          ar & _maxAttempts;
        }
    };
    BOOST_SERIALIZATION_ASSUME_ABSTRACT(GenericStrategy)
}

#endif // GENERICSTRATEGY_H
