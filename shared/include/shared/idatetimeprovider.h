#ifndef IDATETIMEPROVIDER_H
#define IDATETIMEPROVIDER_H

#include <ctime>

namespace Algo {

class IDateTimeProvider
{
public:
    virtual ~IDateTimeProvider() {}
    virtual time_t currentUtcTime() const = 0;
    virtual time_t currentUtcTimeInMsecs() const = 0;
};

}

#endif // IDATETIMEPROVIDER_H
