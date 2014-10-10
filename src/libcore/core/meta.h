#pragma once

#include <cstddef>
#include "platform.h"

namespace atom {

//
// type_of functionality
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
  COMPONENT_TYPE,
  UNKNOWN
//  IMAGE
};

template<typename T>
Type type_of()
{
  return T::missing_type_of_implementation_for_this_type;
}

template<typename T>
inline Type type_of(const T &)
{
  return type_of<T>();
}

/// convenient macro that define type_of function for given type
#define TYPE_OF(type, mapped)   \
  template<>                    \
  inline Type type_of<type>()   \
  { return Type::mapped; }


//
// type_of for basic types
//

TYPE_OF(bool, BOOL)
TYPE_OF(float, FLOAT)
TYPE_OF(int, INT)


//
// Introspection/meta system
//

/**
 * Each meta field is represented by this structure.
 */
struct MetaField {
  const char *name;   ///< field name
  Type        type;   ///< field type
  u32         offset; ///< field offset from structure beginning

  MetaField(const char *field_name, Type field_type, u32 field_offset)
    : name(field_name)
    , type(field_type)
    , offset(field_offset)
  {}
};


/**
 * Each meta class is represented by this structure, contains pointer to parent
 * and pointer to fields.
 */
struct MetaClass {
  const MetaClass *parent;      ///< parent class (nullptr for root classes)
  const char      *name;        ///< class name
  const MetaField *fields;      ///< instance fields
  u32              field_count; ///< number of fields

  MetaClass()
    : parent(nullptr)
    , name(nullptr)
    , fields(nullptr)
    , field_count(0)
  {}

  MetaClass(const char *class_name, const MetaClass *parent_class,
    const MetaField *class_fields, unsigned count);

  const MetaField* find_field(const char *name) const;
};


//
// Macros for declaring/defining meta classes/fields.
//
// This introspection system is based on declaration of special methods to the
// class interface and it's implementation in source file. An introspection
// system can be implemented in header file only, but this approach results
// in bigger library/executable. So meta system use header file for declaration
// and source file to hold data.
//

/**
 * Declare root meta class (class with no parent)
 *
 * Must be placed in public section of class declaration.
 */
#define META_ROOT_CLASS                         \
  const MetaClass *meta;                        \
                                                \
  const MetaClass& meta_class()                 \
  {                                             \
    return *meta;                               \
  }                                             \
                                                \
  static const MetaClass* meta_class_parent()   \
  {                                             \
    return nullptr;                             \
  }                                             \
                                                \
  static const MetaClass* static_meta_class()

/**
 * Declare meta sub class (class with parent)
 *
 * Must be placed in public section of class declaration.
 */
#define META_SUB_CLASS(PARENT_CLASS)            \
  static const MetaClass* meta_class_parent()   \
  {                                             \
    return PARENT_CLASS::static_meta_class();   \
  }                                             \
                                                \
  static const MetaClass* static_meta_class()

/**
 * Define meta class and meta fields
 *
 * Must be placed in source file and class name must be provided.
 * Each META_ROOT_CLASS/META_SUB_CLASS must have a coresponding META_CLASS
 * in source file.
 */
#define META_CLASS(CLASS, ...)                  \
  const MetaClass* CLASS::static_meta_class()   \
  {                                             \
    typedef CLASS class_type;                   \
                                                \
    static MetaField meta_fields[] = {          \
      __VA_ARGS__                               \
    };                                          \
                                                \
    static MetaClass meta_class(#CLASS,         \
      meta_class_parent(),                      \
      meta_fields,                              \
      sizeof(meta_fields) / sizeof(MetaField)); \
                                                \
    return &meta_class;                         \
  }

/**
 * Define meta field
 * 
 * The FIELD macro must be nested in META_CLASS.
 */
#define FIELD(member, name) MetaField(name,     \
  type_of<decltype(class_type::member)>(), offsetof(class_type, member))

/**
 * Initialize instance meta class pointer
 *
 * Must be called in constructor of classes that contains META_ROOT_CLASS
 * or META_SUB_CLASS in declaration.
 */
#define META_INIT() meta = static_meta_class();

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


/**
 * Enumerator to list all fields of meta class (with parent fiels)
 */
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

  MetaObject(const MetaClass &meta, void *ptr);
};

template<typename T>
MetaObject meta_object(T& obj)
{
  return MetaObject(obj.meta_class(), &obj);
}

}
