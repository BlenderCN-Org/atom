#include "meta.h"
#include <cassert>
#include <cstdio>
#include <cstring>
#include "log.h"
#include "string.h"
#include "math/math.h"

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

    my_meta_class = my_meta_class->parent;
    my_index = 0;
  }

  return nullptr;
}

MetaClass::MetaClass(const char *class_name, const MetaClass *parent_class,
  const MetaField *class_fields, unsigned count)
  : parent(parent_class)
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

    case Type::I32:
      *reinterpret_cast<i32 *>(dst) = *reinterpret_cast<const i32 *>(src);
      break;
      
    case Type::U32:
      *reinterpret_cast<u32 *>(dst) = *reinterpret_cast<const u32 *>(src);
      break;

    case Type::F32:
      *reinterpret_cast<f32 *>(dst) = *reinterpret_cast<const f32 *>(src);
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
      
    case Type::COMPONENT_TYPE:
      *reinterpret_cast<ComponentType *>(dst) = *reinterpret_cast<const ComponentType *>(src);
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

MetaObject::MetaObject(const MetaClass &meta, void *ptr)
  : meta_class(meta)
  , data(ptr)
{
  assert(data != nullptr);
}

}
