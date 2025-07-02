#ifndef POSIX_HH_INCLUDED
#define POSIX_HH_INCLUDED

#include <cstdio>

namespace posix {
  int isatty(int fd);
  int fileno(FILE *f);
};

#endif
