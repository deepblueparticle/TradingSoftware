/*
 * File:   defines.h
 * Author: divya
 *
 * Created on May 7, 2015, 10:57 PM
 */

#ifndef DEFINES_H
#define	DEFINES_H

#include <stdint.h>
#include <string.h>
#include <string>
#include <mutex>
#include <cassert>

#ifdef _WIN32
#include <Winsock2.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#endif

#define USERNAME_SIZE       100
#define PASSWORD_SIZE       100
#define PASSWORD_SALT_SIZE  100
#define SYMBOL_SIZE         24
#define TIME_SIZE           24
#define ALERT_MSG_SIZE      500
#define CL_ORDER_SIZE       40

#define MAX_BUF             4096

#define CLIENT_NAME_SIZE     50
#define DAILY_WEEKLY_SIZE    50
#define UPLOAD_FILE_NAME_SIZE    50
#define IDENTIFIER_SIZE 50

#define TICK_MULTIPLIER     100000000
const double TICK_DIVISOR = 100000000.0;
const int PERSIST_FILE_NUM = 20;
const double PERSIST_TOO_SMALL_RATIO = 0.7;


typedef uint8_t UNSIGNED_CHARACTER;
typedef uint16_t UNSIGNED_SHORT;
typedef uint32_t UNSIGNED_INTEGER;
typedef uint64_t UNSIGNED_LONG;
typedef int64_t SIGNED_LONG;

class Profiler {
public:
  Profiler(const std::string&);
  ~Profiler();
private:
  long long _start;
  std::string _funName;
};

#define IS_BIG_ENDIAN (*(uint16_t *)"\0\xff" < 0x100)

#define SWAP16(a) ( \
    ((a & 0x00FF) << 8) | ((a & 0xFF00) >> 8))

#define SWAP32(a) ( \
  ((a & 0x000000FF) << 24) | \
  ((a & 0x0000FF00) <<  8) | \
  ((a & 0x00FF0000) >>  8) | \
  ((a & 0xFF000000) >> 24))

#define SWAP64(a) ( \
    ((a & 0x00000000000000FFULL) << 56) | \
    ((a & 0x000000000000FF00ULL) << 40) | \
    ((a & 0x0000000000FF0000ULL) << 24) | \
    ((a & 0x00000000FF000000ULL) <<  8) | \
    ((a & 0x000000FF00000000ULL) >>  8) | \
    ((a & 0x0000FF0000000000ULL) >> 24) | \
    ((a & 0x00FF000000000000ULL) >> 40) | \
    ((a & 0xFF00000000000000ULL) >> 56))

UNSIGNED_SHORT htons_16(uint16_t x);
UNSIGNED_SHORT ntohs_16(uint16_t x);
UNSIGNED_INTEGER   htonl_32(uint32_t x);
UNSIGNED_INTEGER   ntohl_32(uint32_t x);
UNSIGNED_LONG  htonl_64(uint64_t x);
SIGNED_LONG  signed_htonl_64(int64_t x);
UNSIGNED_LONG  ntohl_64(uint64_t x);
SIGNED_LONG  signed_ntohl_64(int64_t x);

#define IS_LITTLE_ENDIAN (1 != htons(1))

double htond_internal( double value );
float htonf_internal(float value);
double ntohd_internal( double value );
float ntohf_internal(float value);

char *strdup_internal(const char *s);

#define STRCPY_ASSERT(dst, src, dstSize) \
  assert((dstSize) > strlen(src)); \
  strcpy((dst), (src))

#define STRNCPY_ASSERT(dst, src, dstSize) \
  assert((dstSize) > strlen(src)); \
  strncpy((dst), (src), (dstSize))

/////////////////////////////////////////////////////////////

#define SERIALIZE_8(tmp, getMethod, buf, bytes) \
    tmp = getMethod; \
    memcpy(buf+bytes, &tmp, sizeof(tmp)); \
    bytes += sizeof(tmp);

#define SERIALIZE_16(tmp, getMethod, buf, bytes) \
    tmp = htons_16(getMethod);\
    memcpy(buf+bytes, &tmp, sizeof(tmp));\
    bytes += sizeof(tmp);

#define SERIALIZE_32(tmp, getMethod, buf, bytes) \
    tmp = htonl_32(getMethod);\
    memcpy(buf+bytes, &tmp, sizeof(tmp));\
    bytes += sizeof(tmp);

#define SERIALIZE_64(tmp, getMethod, buf, bytes) \
    tmp = htonl_64(getMethod);\
    memcpy(buf+bytes, &tmp, sizeof(tmp));\
    bytes += sizeof(tmp);

#define SERIALIZE_SIGNED_64(tmp, getMethod, buf, bytes) \
    tmp = signed_htonl_64(getMethod);\
    memcpy(buf+bytes, &tmp, sizeof(tmp));\
    bytes += sizeof(tmp);

#define SERIALIZE_DOUBLE(tmp, getMethod, buf, bytes) \
    tmp = htond_internal(getMethod);\
    memcpy(buf+bytes, &tmp, sizeof(tmp));\
    bytes += sizeof(tmp);

#define SERIALIZE_FLOAT(tmp, getMethod, buf, bytes) \
    tmp = htonf_internal(getMethod);\
    memcpy(buf+bytes, &tmp, sizeof(tmp));\
    bytes += sizeof(tmp);


/////////////////////////////////////////////////////////////

#define DESERIALIZE_8(tmp, setMethod, buf, offset)\
    memcpy(&tmp, buf + offset, sizeof(tmp));\
    setMethod;\
    offset  += sizeof(tmp);

#define DESERIALIZE_16(tmp, tmpData, setMethod, buf, offset)\
    memcpy(&tmp, buf + offset, sizeof(tmp));\
    tmp = ntohs_16(tmp);\
    memcpy(&tmpData, &tmp, sizeof(tmpData));\
    setMethod;\
    offset  += sizeof(tmp);

#define DESERIALIZE_32(tmp, tmpData, setMethod, buf, offset)\
    memcpy(&tmp, buf + offset, sizeof(tmp));\
    tmp = ntohl_32(tmp);\
    memcpy(&tmpData, &tmp, sizeof(tmpData));\
    setMethod;\
    offset  += sizeof(tmp);

#define DESERIALIZE_64(tmp, tmpData, setMethod, buf, offset)\
    memcpy(&tmp, buf + offset, sizeof(tmp));\
    tmp = ntohl_64(tmp);\
    memcpy(&tmpData, &tmp, sizeof(tmpData));\
    setMethod;\
    offset  += sizeof(tmp);

#define DESERIALIZE_SIGNED_64(tmp, tmpData, setMethod, buf, offset)\
    memcpy(&tmp, buf + offset, sizeof(tmp));\
    tmp = signed_ntohl_64(tmp);\
    memcpy(&tmpData, &tmp, sizeof(tmpData));\
    setMethod;\
    offset  += sizeof(tmp);

#define DESERIALIZE_DOUBLE(tmp, tmpData, setMethod, buf, offset)\
    memcpy(&tmp, buf + offset, sizeof(tmp));\
    tmp = ntohd_internal(tmp);\
    memcpy(&tmpData, &tmp, sizeof(tmpData));\
    setMethod;\
    offset  += sizeof(tmp);

#define DESERIALIZE_FLOAT(tmp, tmpData, setMethod, buf, offset)\
    memcpy(&tmp, buf + offset, sizeof(tmp));\
    tmp = ntohf_internal(tmp);\
    memcpy(&tmpData, &tmp, sizeof(tmpData));\
    setMethod;\
    offset  += sizeof(tmp);

#ifndef FRONTEND
static std::mutex uniqueStrategyIdMutex;
long generateUniqueStrategyId();
long getCurrentStrategyId();
void initCurrentStrategyId(long id);
void setFirstStrategyId(long id);
long getFirstStrategyId();
std::string getTimeStr();


#else
long getUniqueId();
void UpdateId(long id);
#endif

#endif	/* DEFINES_H */

