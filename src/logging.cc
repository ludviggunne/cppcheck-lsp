#include <stdexcept>
#include <cstdio>
#include <cstdarg>
#include <mutex>

#include "logging.hh"
#include "lsp/error.hh"

static FILE *s_file = stderr;
static log_::Level s_level;

void log_::set_file(const std::string &filename)
{
  if ((s_file = fopen(filename.c_str(), "a")) == NULL)
    throw lsp::Exception(lsp::ErrorCode::InternalError, "Unable to open log file " + filename);
}

void log_::enable(Level level)
{
  s_level = level;
}

static void log_impl(const char *prefix, const char *fmt, va_list ap)
{
  static std::mutex mutex;
  std::lock_guard lg(mutex);

  va_list print_ap;
  va_copy(print_ap, ap);

  std::size_t size = 1 + vsnprintf(NULL, 0, fmt, ap);

  static std::string buffer;
  buffer.reserve(size);

  vsnprintf(&buffer[0], size, fmt, print_ap);
  va_end(print_ap);

  fprintf(s_file, "%s%s\n", prefix, buffer.c_str());
  fflush(s_file);
}

void log_::info(const char *fmt, ...)
{
  if (s_level < log_::Level::Info)
    return;
  va_list ap;
  va_start(ap, fmt);
  log_impl("[info]: ", fmt, ap);
  va_end(ap);
}

void log_::error(const char *fmt, ...)
{
  if (s_level < log_::Level::Error)
    return;
  va_list ap;
  va_start(ap, fmt);
  log_impl("[error]: ", fmt, ap);
  va_end(ap);
}

void log_::debug(const char *fmt, ...)
{
  if (s_level < log_::Level::Debug)
    return;
  va_list ap;
  va_start(ap, fmt);
  log_impl("[debug]: ", fmt, ap);
  va_end(ap);
}
