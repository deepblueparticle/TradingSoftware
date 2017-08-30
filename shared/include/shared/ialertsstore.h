#ifndef IALERTSSTORE_H
#define IALERTSSTORE_H

#include <string>

namespace Algo {

class Alerts;

class IAlertsStore
{
public:
    virtual ~IAlertsStore() {}
    virtual void addAlert(const Alerts &alert) = 0;
};

}

#endif // IALERTSSTORE_H
