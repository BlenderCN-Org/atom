#pragma once

#include <cstddef>
#include "platform.h"

//QQQ komentar pre meta a types

namespace atom {

//
// TYPES
//

enum class Type : u32 {
  BOOL,
  INT,
  FLOAT,
  I8,
  U8,
  I16,
  U16,
  I32,
  U32,
  I64,
  U64,
  F32,
  F64,
  VEC2F,
  VEC3F,
  VEC4F,
  VEC4U8,
  MAT2F,
  MAT3F,
  MAT4F,
  MAT4F_ARRAY,
  STRING,
  SHADER,
  TEXTURE,
  SAMPLER_2D,
  DRAW_FACE,
  UNKNOWN
//  IMAGE
};

template<typename T>
Type type_of()
{
  return T::type;
}

template<typename T>
inline Type type_of(const T &)
{
  return type_of<T>();
}

// toto makro nadefinuje mapovaciu sablonu type_of pre dany typ
#define MAP_TYPE(type, mapped)  \
  template<>                    \
  inline Type type_of<type>()   \
  { return Type::mapped; }

MAP_TYPE(bool, BOOL)
MAP_TYPE(float, FLOAT)
MAP_TYPE(int, INT)

//
// META
//

struct MetaField {
  const char *name;
  Type        type;
  u32         offset;

  MetaField(const char *field_name, Type field_type, u32 field_offset)
    : name(field_name)
    , type(field_type)
    , offset(field_offset)
  {}
};

template<typename T>
void* ptr_with_offset(void *ptr, T offset)
{
  return reinterpret_cast<char *>(ptr) + offset;
}

/// to iste co predchadzajuce len const verzia
template<typename T>
const void* ptr_with_offset(const void *ptr, T offset)
{
  return reinterpret_cast<const char *>(ptr) + offset;
}
// get reference to specific field with specific type
template<typename T>
inline T& field_ref(const MetaField &p, void *data)
{
  return *reinterpret_cast<T *>(ptr_with_offset(data, p.offset));
}
// const version of field_ref
template<typename T>
inline const T& field_ref(const MetaField &p, const void *data)
{
  return *reinterpret_cast<const T *>(ptr_with_offset(data, p.offset));
}

struct MetaClass {
  const MetaClass *parent_class;
  const char      *name;
  const MetaField *fields;
  unsigned   field_count;

  MetaClass()
    : parent_class(nullptr)
  {}

  MetaClass(const MetaClass *parent, const char *class_name, const MetaField *class_fields, unsigned count);

  const MetaField* find_field(const char *name) const;
};

/// pridaj meta informacie o clenskych premennych
#define META_DEFINE_FIELDS(type)  \
  MetaField type::meta_fields[] =

#define META_FIELD(type, member, name) \
  MetaField(name, type_of<decltype(type::member)>(), offsetof(type, member))

/// popis clena triedy, musi byt vnorene v META_DEFINE_FIELDS
/// META_CLASS_NAME je nazov triedy (definicia v DEFINE_META_FIELDS)
#define FIELD(type, member, name) \
  META_FIELD(type, member, name)

/// pridaj do (deklaracie) triedy meta informacie
#define META_DECLARE_CLASS        \
  static MetaField meta_fields[]; \
  static MetaClass meta_class

/// pridaj ukazovatel na meta info do kazdej instancie
#define META_DECLARE_CLASS_PTR MetaClass *meta

/// pridaj do (definicie) triedy meta informacie
/// vhodne pre odvodenu triedu
#define META_DEFINE_CLASS(C, PARENT_CLASS, NAME) \
  MetaClass C::meta_class(&PARENT_CLASS::meta_class, NAME, C::meta_fields, \
    sizeof(C::meta_fields) / sizeof(MetaField))

/// pridaj do (definicie) triedy meta informacie
/// vhodne pre najvyssiu (nie odvodenu) triedu
#define META_DEFINE_ROOT_CLASS(C, NAME) \
  MetaClass C::meta_class(nullptr, NAME, C::meta_fields, \
    sizeof(C::meta_fields) / sizeof(MetaField))

/// inicializuj meta ptr (kazda instancia ma svoj meta ptr)
/// &meta_class je adresa statickej clenskej premennej s meta informaciami
#define META_INIT() meta = &meta_class;

struct MetaFieldsEnumarator {
  explicit MetaFieldsEnumarator(const MetaClass &meta);

  const MetaField* next();

private:
  const MetaClass *my_meta_class;
  unsigned         my_index;
};

/**
 * Funkcia na kopirovanie FIELD-ov (ich hodnot).
 */
void copy_field_values(const MetaClass &src_meta, const MetaClass &dst_meta,
  const void *src, void *dst);

/**
 * Convenient wrapper for copy_field_values function. Need only 2 arguments.
 */
template<typename S, typename D>
void copy_field_values(const S &src, D &dst)
{
  copy_field_values(*src.meta, *dst.meta, &src, &dst);
}


struct MetaObject {
  const MetaClass &meta_class;
  void            *data;

  MetaObject(MetaClass &meta, void *ptr);
};

template<typename T>
MetaObject meta_object(T& obj)
{
  return MetaObject(obj.meta_class, &obj);
}

}
