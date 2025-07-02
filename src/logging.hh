#ifndef LOGGING_HH_INCLUDED
#define LOGGING_HH_INCLUDED

#include <string>

namespace log_ {

  enum class Level : unsigned {
    None = 0,
    Error = 1,
    Info  = 2,
    Debug = 3,
  };

  void set_file(const std::string &filename);
  void enable(Level level);

  void info(const char *fmt, ...);
  void error(const char *fmt, ...);
  void debug(const char *fmt, ...);
};

#endif
