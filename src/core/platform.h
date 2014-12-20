#pragma once

#include <cstdint>

#if defined(__GNUC__) || defined(__clang__)
  #define ATOM_FUNC_NAME __PRETTY_FUNCTION__
#elif defined(_MSC_VER)
  #define ATOM_FUNC_NAME __FUNCSIG__
#else
  #error Unsupported compiler
#endif

namespace atom {

typedef unsigned uint;

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

const i8   I8_MAX = 127;
const i16 I16_MAX = 32767;
const i32 I32_MAX = 2147483647;
const i64 I64_MAX = 9223372036854775807L;
const f32 F32_MAX = 1e37;
const f64 F64_MAX = 1e37;


const i8  I8_MIN  = -I8_MAX  - 1;
const i16 I16_MIN = -I16_MAX - 1;
const i32 I32_MIN = -I32_MAX - 1;
const i64 I64_MIN = -I64_MAX - 1;
const f32 F32_MIN = 1e-37;
const f64 F64_MIN = 1e-37;

static_assert(I8_MIN  < I8_MAX , "Min has to be smaller than max");
static_assert(I16_MIN < I16_MAX, "Min has to be smaller than max");
static_assert(I32_MIN < I32_MAX, "Min has to be smaller than max");
static_assert(I64_MIN < I64_MAX, "Min has to be smaller than max");

const u32 U32_MAX = 0xFFFFFFFF;


// hodnota float je v OpenGL GLfloat
#define INDEX_GL_TYPE GL_UNSIGNED_INT

}
