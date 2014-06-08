#include "field_value.h"

namespace atom {
namespace editor {

FieldValue::FieldValue(Type type)
  : my_type(type)
{
}

Type FieldValue::type() const
{
  return my_type;
}

}
}
