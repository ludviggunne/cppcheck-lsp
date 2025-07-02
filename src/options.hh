#ifndef OPTIONS_HH_INCLUDED
#define OPTIONS_HH_INCLUDED

#include <string>
#include <vector>
#include <ostream>

#include "logging.hh"

struct Options {
  void parse_cli(int argc, char **argv);
  static void show_help(std::ostream &out);

  std::string logfile = "";
  log_::Level loglevel = log_::Level::None;
  std::string cppcheck_path = "cppcheck";
  std::vector<std::string> cppcheck_args;

private:
  void long_option(const std::string &arg);
};

#endif
