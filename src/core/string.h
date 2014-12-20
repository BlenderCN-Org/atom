#pragma once

#include <string>
#include "meta.h"

namespace atom {

typedef std::string String;

bool is_blank(const String &string);
bool is_comment(const String &string);

template<>
inline Type type_of<String>()
{
  return Type::STRING;
}

}
