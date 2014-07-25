#pragma once

#include <QString>
#include <core/ptr.h>
#include <core/string.h>
//#include "field_value.h"
#include "editorfwd.h"

namespace atom {
namespace editor {

inline QString to_qstring(const String &str)
{
  return QString::fromStdString(str);
}

uptr<FieldValue> create_field_value_for_type(Type type);

}
}
