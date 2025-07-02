#ifndef SUBPROCESS_HH_INCLUDED
#define SUBPROCESS_HH_INCLUDED

#include <vector>
#include <string>

namespace subprocess {

  struct Result {
    int exitcode = 0;

    /* Lines of output (stderr & stdout) */
    std::vector<std::string> output;
  };

  Result run(const char **argv);
};

#endif
