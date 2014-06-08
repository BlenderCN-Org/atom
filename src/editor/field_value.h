#pragma once

#include <core/noncopyable.h>
#include <core/math/vec.h>
#include <core/string.h>

namespace atom {
namespace editor {

class FieldValue : NonCopyable {
public:
  explicit FieldValue(Type type);

  virtual ~FieldValue()
  {
  }

  virtual void load_value_from(const void *data) = 0;

  virtual void set_value_to(void *data) const = 0;

  Type type() const;

private:
  Type my_type;

};

template<typename T>
class SimpleFieldValue : public FieldValue {
public:
  SimpleFieldValue()
    : FieldValue(type_of<T>())
  {
  }

  explicit SimpleFieldValue(const T &value)
    : FieldValue(type_of<T>())
    , my_value(value)
  {
  }

  virtual void load_value_from(const void *data)
  {
    my_value = *reinterpret_cast<const T *>(data);
  }

  virtual void set_value_to(void *data) const
  {
    *reinterpret_cast<T *>(data) = my_value;
  }

  const T& value() const
  {
    return my_value;
  }

private:
  T my_value;
};

typedef SimpleFieldValue<int> FieldValueInt;
typedef SimpleFieldValue<float> FieldValueFloat;
typedef SimpleFieldValue<Vec2f> FieldValueVec2f;
typedef SimpleFieldValue<String> FieldValueString;

}
}
