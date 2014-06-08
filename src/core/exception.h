#pragma once

#include "core/string.h"

namespace atom {

class Exception : public std::exception {
public:
  Exception(const char *message)
    : my_error_string(message)
  {}

  ~Exception() throw()
  {}

  const char* what() const throw()
  { return my_error_string.c_str(); }

private:
  String my_error_string;
};

}
