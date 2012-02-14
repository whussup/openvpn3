#ifndef OPENVPN_LOG_SESSIONSTATS_H
#define OPENVPN_LOG_SESSIONSTATS_H

#include <cstring>

#include <openvpn/common/types.hpp>
#include <openvpn/common/rc.hpp>
#include <openvpn/error/error.hpp>

namespace openvpn {

  class SessionStats : public RC<thread_safe_refcount>
  {
  public:
    typedef boost::intrusive_ptr<SessionStats> Ptr;

    enum Stats {
      // operating stats
      BYTES_IN = 0,
      BYTES_OUT,
      TUN_BYTES_IN,
      TUN_BYTES_OUT,
      N_STATS,
    };

    SessionStats()
    {
      std::memset(stats_, 0, sizeof(stats_));
    }

    virtual void error(const size_t type, const std::string* text=NULL) = 0;

    void inc_stat(const size_t type, const count_t value)
    {
      if (type < N_STATS)
	stats_[type] += value;
    }

    count_t get_stat(const size_t type) const
    {
      if (type < N_STATS)
	return stats_[type];
      else
	return 0;
    }

    static const char *stat_name(const size_t type)
    {
      static const char *names[] = {
	"BYTES_IN",
	"BYTES_OUT",
	"TUN_BYTES_IN",
	"TUN_BYTES_OUT",
      };

      if (type < N_STATS)
	return names[type];
      else
	return "UNKNOWN_STAT_TYPE";
    }

  private:
    count_t stats_[N_STATS];
  };

} // namespace openvpn

#endif // OPENVPN_LOG_SESSIONSTATS_H