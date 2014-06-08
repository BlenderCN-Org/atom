#include "meta.h"
#include <cassert>
#include <cstdio>
#include <cstring>
#include "core/log.h"
#include "core/string.h"
#include "core/math/vec.h"

namespace atom {

const MetaField* MetaClass::find_field(const char *name) const
{
  assert(name != nullptr);

  MetaFieldsEnumarator enumerator(*this);

  const MetaField *meta_field;

  while ((meta_field = enumerator.next())) {
    if (!strcmp(meta_field->name, name)) {
      return meta_field;
    }
  }

  return nullptr;
}

MetaFieldsEnumarator::MetaFieldsEnumarator(
    const MetaClass &meta)
  : my_meta_class(&meta)
  , my_index(0)
{
}

const MetaField* MetaFieldsEnumarator::next()
{
  while (my_meta_class != nullptr) {
    if (my_index < my_meta_class->field_count)
      return &(my_meta_class->fields[my_index++]);

    my_meta_class = my_meta_class->parent_class;
    my_index = 0;
  }

  return nullptr;
}

MetaClass::MetaClass(const MetaClass *parent, const char *class_name,
  const MetaField *class_fields, unsigned count)
  : parent_class(parent)
  , name(class_name)
  , fields(class_fields)
  , field_count(count)
{

}

void set_value(Type type, const void *src, void *dst)
{
  switch (type) {
    case Type::BOOL:
      *reinterpret_cast<bool *>(dst) = *reinterpret_cast<const bool *>(src);
      break;

    case Type::INT:
      *reinterpret_cast<int *>(dst) = *reinterpret_cast<const int *>(src);
      break;

    case Type::FLOAT:
      *reinterpret_cast<float *>(dst) = *reinterpret_cast<const float *>(src);
      break;

    case Type::STRING:
      *reinterpret_cast<String *>(dst) = *reinterpret_cast<const String *>(src);
      break;

    case Type::VEC2F:
      *reinterpret_cast<Vec2f *>(dst) = *reinterpret_cast<const Vec2f *>(src);
      break;

    case Type::VEC3F:
      *reinterpret_cast<Vec3f *>(dst) = *reinterpret_cast<const Vec3f *>(src);
      break;

    default:
      log::warning("Unsupported type %i", type);
      break;
  }
}

void copy_field_values(const MetaClass &src_meta, const MetaClass &dst_meta, const void *src, void *dst)
{
  assert(src != nullptr);
  assert(dst != nullptr);

  MetaFieldsEnumarator src_fields(src_meta);
  const MetaField *src_field;

  while ((src_field = src_fields.next())) {
    const MetaField *dst_field = dst_meta.find_field(src_field->name);

    if (dst_field == nullptr) {
      // skip non existing field
      continue;
    }

    set_value(src_field->type,
      ptr_with_offset(src, src_field->offset),
      ptr_with_offset(dst, dst_field->offset));
  }
}

}
