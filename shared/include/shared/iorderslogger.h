#ifndef IORDERSLOGGER_H
#define IORDERSLOGGER_H

namespace Algo {

class Order;

class IOrdersLogger
{
public:
    virtual ~IOrdersLogger() {}
    virtual void logOrder(const Order &order) = 0;
};

}

#endif // IORDERSLOGGER_H
