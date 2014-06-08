#include "core/log.h"

#include <cstdio>
#include <cstdarg>
#include <mutex>
#include "core/string.h"

namespace atom {
namespace log {

namespace {

String color_white;
String color_yellow;
String color_red;
String color_end;

std::mutex log_mutex;
}

void set_color_enabled(bool enabled)
{
  if (enabled) {
    color_white  = "\033[0m"; //"\033[37m";
    color_yellow = "\033[1;33m";
    color_red    = "\033[1;31m";
    color_end    = "\033[0m";
  } else {
    color_white  = String();
    color_yellow = String();
    color_red    = String();
    color_end    = String();
  }
}

void error(const char *format, ...)
{
  std::lock_guard<std::mutex> lock(log_mutex);
  va_list ap;
  va_start(ap, format);
  fprintf(stderr, "%s", color_red.c_str());
  fprintf(stderr, "!!! ");
  vfprintf(stderr, format, ap);
  fprintf(stderr, "%s\n", color_end.c_str());
  va_end(ap);
}

void info(const char *format, ...)
{
  std::lock_guard<std::mutex> lock(log_mutex);
  va_list ap;
  va_start(ap, format);
  fprintf(stdout, "%s", color_white.c_str());
  vfprintf(stdout, format, ap);
  fprintf(stdout, "%s\n", color_end.c_str());
  va_end(ap);

  fflush(stdout);
}

void warning(const char *format, ...)
{
  std::lock_guard<std::mutex> lock(log_mutex);

  va_list ap;
  va_start(ap, format);
  fprintf(stderr, "%s", color_yellow.c_str());
  fprintf(stderr, "!!! ");
  vfprintf(stderr, format, ap);
  fprintf(stderr, "%s\n", color_end.c_str());
  va_end(ap);

  fflush(stdout);
}

#ifndef NDEBUG

void debug(bool print, const char *format, ...)
{
  if (!print)
    return;
  /// @todo copied from info function
  std::lock_guard<std::mutex> lock(log_mutex);
  va_list ap;
  va_start(ap, format);
  fprintf(stdout, "%s", color_white.c_str());
  vfprintf(stdout, format, ap);
  fprintf(stdout, "%s\n", color_end.c_str());
  va_end(ap);

  fflush(stdout);
}

#endif

}

void not_implemented_message(const char *msg)
{
  std::lock_guard<std::mutex> lock(log::log_mutex);
  if (msg != nullptr)
    printf("This is not implemented yet %s\n", msg);
  else
    printf("This is not implemented yet\n");
}

void not_tested_message(const char *msg)
{
  std::lock_guard<std::mutex> lock(log::log_mutex);
  if (msg != nullptr)
    printf("This is not tested yet: %s\n", msg);
  else
    printf("This is not tested yet\n");
}

void subclass_responsibility_message(const char *msg)
{
  std::lock_guard<std::mutex> lock(log::log_mutex);
  if (msg != nullptr)
    printf("This method must be implemented in subclass: %s\n", msg);
  else
    printf("This method must be implemented in subclass\n");
}

}
