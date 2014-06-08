#include "utils.h"
#include <QStringList>
#include "editor/field_value.h"
#include "editor/log.h"

namespace atom {
namespace editor {

uptr<FieldValue> create_field_value_for_type(Type type)
{
  uptr<FieldValue> field_value;

  switch (type) {
    case Type::INT:
      field_value.reset(new FieldValueInt());
      break;

    case Type::FLOAT:
      field_value.reset(new FieldValueFloat());
      break;

    case Type::STRING:
      field_value.reset(new FieldValueString());
      break;

    case Type::VEC2F:
      field_value.reset(new FieldValueVec2f());
      break;

    default:
      log::warning("This field type \"%i\" is not supported in the %s", type, MM_FUNC_NAME);
      break;
  }

  return field_value;
}

}
}

