#include <unistd.h>

#include "posix.hh"

int posix::isatty(int fd)
{
  return ::isatty(fd);
}

int posix::fileno(FILE *f)
{
  return ::fileno(f);
}
