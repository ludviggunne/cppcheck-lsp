#include <io.h>

#include "posix.hh"

int posix::isatty(int fd)
{
  return ::_isatty(fd);
}

int posix::fileno(FILE *f)
{
  return ::_fileno(f);
}
