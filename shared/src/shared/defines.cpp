#include <shared/defines.h>

#include <string.h>
#include <iostream>
#include <errno.h>
#include <ctime>
//#include <boost/date_time/posix_time/posix_time.hpp>


UNSIGNED_SHORT htons_16(uint16_t x)
{
    return(IS_BIG_ENDIAN?x:SWAP16(x));
}

UNSIGNED_SHORT ntohs_16(uint16_t x)
{
    return(IS_BIG_ENDIAN?x:SWAP16(x));
}

UNSIGNED_INTEGER htonl_32(uint32_t x)
{
    return(IS_BIG_ENDIAN?x:SWAP32(x));
}

UNSIGNED_INTEGER ntohl_32(uint32_t x)
{
    return(IS_BIG_ENDIAN?x:SWAP32(x));
}

UNSIGNED_LONG htonl_64(uint64_t x)
{
    return(IS_BIG_ENDIAN?x:SWAP64(x));
}

UNSIGNED_LONG ntohl_64(uint64_t x)
{
    return(IS_BIG_ENDIAN?x:SWAP64(x));
}

SIGNED_LONG signed_htonl_64(int64_t x)
{
    return(IS_BIG_ENDIAN?x:SWAP64(x));
}

SIGNED_LONG signed_ntohl_64(int64_t x)
{
    return(IS_BIG_ENDIAN?x:SWAP64(x));
}
Profiler::Profiler(const std::string& funName_) : _funName(funName_)
{
  std::chrono::nanoseconds start_ms = 
    std::chrono::duration_cast< std::chrono::nanoseconds >(std::chrono::system_clock::now().time_since_epoch());
  _start = start_ms.count();
}
Profiler::~Profiler()
{
  std::chrono::nanoseconds end_ms = 
    std::chrono::duration_cast< std::chrono::nanoseconds >(std::chrono::system_clock::now().time_since_epoch());
  std::cout << _funName << " started at (nano) " << _start << " ended at " << end_ms.count() << 
    " took " << end_ms.count() - _start << " nanoseconds." << std::endl;
}

double htond_internal( double value )
{
    uint64_t _value;
    if (IS_LITTLE_ENDIAN)
    {
        _value = *((uint64_t *)&value);
        uint32_t high_part = htonl((uint32_t)(_value >> 32));
        uint32_t low_part = htonl((uint32_t)(_value & 0xFFFFFFFFLL));
        _value = ((((uint64_t)low_part) << 32) | high_part);
        return *(double *)&_value;
    }
    else
    {
        return value;
    }
}

float htonf_internal(float value)
{
    uint32_t _value;
    if (IS_LITTLE_ENDIAN)
    {
        _value = *(uint32_t *)&value;
        uint32_t high_part = htons((uint16_t)(_value >> 16));
        uint32_t low_part = htons((uint16_t)(_value & 0xFFFFL));
        _value = (((uint32_t)low_part) << 16) | high_part;
        return *(float *)&_value;
    }
    else
    {
        return value;
    }
}

double ntohd_internal( double value )
{
    uint64_t _value;
    if (IS_LITTLE_ENDIAN)
    {
        _value = *(uint64_t *)&value;
        uint32_t high_part = ntohl((uint32_t)(_value >> 32));
        uint32_t low_part = ntohl((uint32_t)(_value & 0xFFFFFFFFLL));
        _value = ((((uint64_t)low_part) << 32) | high_part);
        return *(double *)&_value;
    }
    else
    {
        return value;
    }
}

float ntohf_internal(float value)
{
    uint32_t _value;
    if (IS_LITTLE_ENDIAN)
    {
        _value = *(uint32_t *)&value;
        uint32_t high_part = ntohs((uint16_t)(_value >> 16));
        uint32_t low_part = ntohs((uint16_t)(_value & 0xFFFFL));
        _value = (((uint32_t)low_part) << 16) | high_part;
        return *(float *)&_value;
    }
    else
    {
        return value;
    }
}

char *strdup_internal(const char *s)
{
#ifdef _WIN32
    return _strdup(s);
#else
    return strdup(s);
#endif
}
#ifndef FRONTEND
static long orderIdCounter = 0;
static long firstStrategyId = 0;

long generateUniqueStrategyId()
{
  //const int maxOrdersPerSecond = 10000;
  //const int digitsForSeconds = 100000;

  // Take lock
  std::lock_guard<std::mutex> lock(uniqueStrategyIdMutex);
  //long orderId = std::time(nullptr) % digitsForSeconds +
  //        orderIdCounter * digitsForSeconds;
  ++orderIdCounter;
  //if (orderIdCounter == maxOrdersPerSecond)
  //    orderIdCounter = 0;
  return orderIdCounter;
}

long getCurrentStrategyId()
{
	std::lock_guard<std::mutex> lock(uniqueStrategyIdMutex);
	return orderIdCounter;
}

void initCurrentStrategyId(long id)
{
	std::lock_guard<std::mutex> lock(uniqueStrategyIdMutex);
	orderIdCounter = id;
}

void setFirstStrategyId(long id)
{
	firstStrategyId = id;
}

long getFirstStrategyId()
{
	return firstStrategyId;
}

std::string getTimeStr()
{
  namespace bt = boost::posix_time;

  bt::ptime now = boost::posix_time::microsec_clock::universal_time();
  std::stringstream stream;
  boost::posix_time::time_facet* facet = new boost::posix_time::time_facet();
  facet->format("%Y%m%d-%H:%M:%S%F");
  stream.imbue(std::locale(std::locale::classic(), facet));
  stream << now;
  return stream.str();
}

#else
static long uniqueid = 0;

void UpdateId(long id)
{
    if(uniqueid<id)
        uniqueid=id;
}

long getUniqueId()
{
    ++uniqueid;
    return uniqueid;
}

#endif
