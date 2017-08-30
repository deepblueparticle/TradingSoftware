#include <shared/time_util.h>
namespace bt = boost::posix_time;


// http://stackoverflow.com/questions/3786201/how-to-parse-date-time-from-string
// This creates epoch time from string time
namespace Algo {
  std::time_t seconds_from_epoch(const std::string& s)
  {
     bt::ptime pt;
     std::locale format(std::locale::classic(),new bt::time_input_facet("%Y%m%d-%H:%M:%S"));
     std::istringstream is(s);
     is.imbue(format);
     is >> pt;
     if(pt != bt::ptime()) {
          bt::ptime timet_start(boost::gregorian::date(1970,1,1));
         bt::time_duration diff = pt - timet_start;
         return diff.ticks() / bt::time_duration::rep_type::ticks_per_second;
     }
  }

  bt::ptime timeFromString(const std::string& s)
  {
     bt::ptime pt;
     std::locale format(std::locale::classic(),new bt::time_input_facet("%Y%m%d-%H:%M:%S%F"));
     std::istringstream is(s);
     is.imbue(format);
     is >> pt;
     if(pt != bt::ptime()) {
        return pt;
     }
  }
}
