#pragma once

#include <cstdint>

namespace atom {

typedef uint32_t Index;

typedef unsigned uint;
typedef float float32_t;

typedef   int8_t  i8;
typedef  uint8_t  u8;
typedef  int16_t i16;
typedef uint16_t u16;
typedef  int32_t i32;
typedef uint32_t u32;
typedef  int64_t i64;
typedef uint64_t u64;
typedef    float f32;
typedef   double f64;

static_assert(sizeof(i8)  == 1, "Incorrect size of i8");
static_assert(sizeof(u8)  == 1, "Incorrect size of u8");
static_assert(sizeof(i16) == 2, "Incorrect size of i16");
static_assert(sizeof(u16) == 2, "Incorrect size of u16");
static_assert(sizeof(i32) == 4, "Incorrect size of i32");
static_assert(sizeof(u32) == 4, "Incorrect size of u32");
static_assert(sizeof(i64) == 8, "Incorrect size of i64");
static_assert(sizeof(u64) == 8, "Incorrect size of u64");
static_assert(sizeof(f32) == 4, "Incorrect size of f32");
static_assert(sizeof(f64) == 8, "Incorrect size of f64");
//static_assert(sizeof(GLint) == sizeof(int32_t), "Pravdepodobne GLint by mal mat 32 bitov, rovnako ako int32_t");
//static_assert(sizeof(GLuint) == sizeof(uint32_t), "Pravdepodobne GLint by mal mat 32 bitov, rovnako ako int32_t");

const i8   I8_MAX =  127;
const i16 I16_MAX =  32767;
const i32 I32_MAX =  2147483647;
const i64 I64_MAX =  9223372036854775807L;

const i8  I8_MIN  = -I8_MAX  - 1;
const i16 I16_MIN = -I16_MAX - 1;
const i32 I32_MIN = -I32_MAX - 1;
const i64 I64_MIN = -I64_MAX - 1;


static_assert(I8_MIN  < I8_MAX , "Min has to be smaller than max");
static_assert(I16_MIN < I16_MAX, "Min has to be smaller than max");
static_assert(I32_MIN < I32_MAX, "Min has to be smaller than max");
static_assert(I64_MIN < I64_MAX, "Min has to be smaller than max");


// hodnota float je v OpenGL GLfloat
#define INDEX_GL_TYPE GL_UNSIGNED_INT

const float EPSILON = 0.01f;

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
  MAT2F,
  MAT3F,
  MAT4F,
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

}
