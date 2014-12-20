#include "string.h"

namespace atom {

bool is_blank(const String &string)
{
  if (string.empty())
    return true;

  auto i = string.begin();
  auto e = string.end();

  for (; i != e; ++i) {
    if (*i != ' ' && *i != '\t' && *i != '\n' && *i != '\r')
      break;
  }

  return i == e;
}

bool is_comment(const String &string)
{
  if (string.empty())
    return false;

  return string[0] == '#';
}

} // end of MM
