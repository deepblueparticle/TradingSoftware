#include <util/TimeUtil.H>
#include <config/Configuration.H>
#include <ctime>
#include <chrono>
#include <string>
#include <sstream>
#include <iomanip>

namespace Algo
{
  namespace TimeUtil
  {
    bt::ptime getCurrentTime()
    {
      bt::ptime now = boost::posix_time::microsec_clock::universal_time();
      boost::local_time::local_date_time nyNow(
        now, Configuration::getInstance()->getTZNewYork());

      return nyNow.local_time();
    }

    std::string getCurrentTimeStr()
    {
      bt::ptime now = getCurrentTime();
      std::stringstream stream;
      boost::posix_time::time_facet* facet = new boost::posix_time::time_facet();
      facet->format("%Y%m%d-%H:%M:%S%F");
      stream.imbue(std::locale(std::locale::classic(), facet));
      stream << now;
      return stream.str();
    }

    time_t getCurrentTimeEpoch()
    {
      bt::ptime now = getCurrentTime();
      return boost::posix_time::to_time_t(now);
    }

    unsigned int getMilliSecondsFromStartOfDay()
    {
      using namespace std::chrono;
      auto now = system_clock::now();

      // tt stores time in seconds since epoch
      std::time_t tt = system_clock::to_time_t(now);

      // broken time as of now
      std::tm bt = *std::localtime(&tt);

      // alter broken time to the beginning of today
      bt.tm_hour = 0;
      bt.tm_min = 0;
      bt.tm_sec = 0;

      // convert broken time back into std::time_t
      tt = std::mktime(&bt);

      // start of today in system_clock units
      auto start_of_today = system_clock::from_time_t(tt);

      // today's duration in system clock units
      auto length_of_today = now - start_of_today;

      // milliseconds since start of today
      milliseconds ms = duration_cast<milliseconds>(length_of_today);
      return ms.count();
    }

    void getDayMonthYear(int &day, int &month, int &year)
    {
      bt::ptime now = getCurrentTime();
      boost::gregorian::date d = now.date();
      boost::gregorian::date::ymd_type ymd = d.year_month_day();
      day = ymd.day;
      month = ymd.month;
      year = ymd.year;
    }
  }
}
