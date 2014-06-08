#pragma once

#include <cstddef>
#include "core/types.h"

namespace atom {

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

template<typename T>
inline T& field_ref(const MetaField &p, void *data)
{
  return *reinterpret_cast<T *>(ptr_with_offset(data, p.offset));
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

}
