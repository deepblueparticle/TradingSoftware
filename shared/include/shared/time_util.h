#ifndef TIME_UTIL
#define TIME_UTIL

#include <boost/date_time.hpp>

namespace Algo {
  std::time_t seconds_from_epoch(const std::string& s);
  boost::posix_time::ptime timeFromString(const std::string& s);
}

#endif
