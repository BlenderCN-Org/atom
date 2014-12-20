#pragma once

#include "platform.h"
#include "meta.h"
#include "slice.h"
#include "vec.h"
#include "mat.h"
#include "quat.h"

namespace atom {

//
// Vector specialization for most useful types
//
typedef Vec2<u32> Vec2u;
typedef Vec2<i32> Vec2i;
typedef Vec3<u32> Vec3u;
typedef Vec3<i32> Vec3i;
typedef Vec4<u8> Vec4u8;
typedef Vec2<f32> Vec2f;
typedef Vec3<f32> Vec3f;
typedef Vec4<f32> Vec4f;

static_assert(sizeof(Vec2u) == 8, "Velkost musi byt 8bytov");
static_assert(sizeof(Vec2i) == 8, "Velkost musi byt 8bytov");
static_assert(sizeof(Vec3u) == 12, "Velkost musi byt 12bytov");
static_assert(sizeof(Vec3i) == 12, "Velkost musi byt 12bytov");
static_assert(sizeof(Vec2f) == 8, "Velkost musi byt 8bytov");
static_assert(sizeof(Vec3f) == 12, "Velkost musi byt 8bytov");
static_assert(sizeof(Vec4f) == 16, "Velkost musi byt 8bytov");

TYPE_OF(Vec2f, VEC2F)
TYPE_OF(Vec3f, VEC3F)
TYPE_OF(Vec4f, VEC4F)


//
// Matrix specialization for most useful types
//

typedef Mat2<f32> Mat2f;
typedef Mat3<f32> Mat3f;
typedef Mat4<f32> Mat4f;

TYPE_OF(Mat2f, MAT2F)
TYPE_OF(Mat3f, MAT3F)
TYPE_OF(Mat4f, MAT4F)

template<typename T>
const T& min(const T& a, const T &b)
{
  return b > a ? a : b;
}

template<typename T>
const T& max(const T& a, const T& b)
{
  return b < a ? a : b;
}


//
// Quaternion using float (quaternions using double precision aren't used).
//

typedef Quat<f32> Quatf;
static_assert(sizeof(Quatf) == 16, "Size of the Quaterion<float> should be 16 bytes");

/**
 * Equivalent of m * Vec4(v, 1).
 */
template<typename T>
Vec3<T> transform_point(const Mat4<T> &m, const Vec3<T> &v)
{
  T x = m(0, 0) * v[0] + m(0, 1) * v[1] + m(0, 2) * v[2] + m(0, 3);
  T y = m(1, 0) * v[0] + m(1, 1) * v[1] + m(1, 2) * v[2] + m(1, 3);
  T z = m(2, 0) * v[0] + m(2, 1) * v[1] + m(2, 2) * v[2] + m(2, 3);
  T w = m(3, 0) * v[0] + m(3, 1) * v[1] + m(3, 2) * v[2] + m(3, 3);
  return Vec3<T>(x / w, y / w, z / w);
}

/**
 * Equivalent of m * Vec4(v, 0).
 */
template<typename T>
inline Vec3<T> transform_vec(const Mat4<T> &m, const Vec3<T> &v)
{
  T x = m(0, 0) * v[0] + m(0, 1) * v[1] + m(0, 2) * v[2];
  T y = m(1, 0) * v[0] + m(1, 1) * v[1] + m(1, 2) * v[2];
  T z = m(2, 0) * v[0] + m(2, 1) * v[1] + m(2, 2) * v[2];
  return Vec3<T>(x, y, z);
}

/**
 * Vypocitaj koeficienty roviny (a, b, c, d) z troch vrcholov.
 *
 * Vypocitane koeficienty su ulozene v stvorprvkovom vektore v poradi a, b, c, d.
 * Z koeficientov a, b, c sa da ziskat normalovy vektor roviny (Vec3(a, b, c)).
 */
Vec4f plane_from_vertices(const Vec3f &v0, const Vec3f &v1, const Vec3f &v2);

template<typename T>
T bezier(const T &p0, const T &p1, const T &p2, const T &p3, typename T::value_type t)
{
  typedef typename T::value_type V;
  V u = 1 - t;
  V u2 = u * u;
  V u3 = u * u2;
  V t2 = t * t;
  V t3 = t * t2;

  if (t < 0)
    return p0;
  else if (t > 1)
    return p3;
  else
    return (u3 * p0) + (3 * u2 * t * p1) + (3 * u * t2 * p2) + (t3 * p3);
}

template<typename T>
T clamp(T x, T min, T max)
{
  return x < min ? min : (x > max ? max : x);
}

template<typename T>
T lerp(T x, T min, T max)
{
  return min + x * (max - min);
}

template<typename T>
T abs(const T x)
{
  return x < 0 ? -x : x;
}

/**
 * @note: this is branchless sign function
 *
 * @return 1 when x > 0
 *         0 when x == 0
 *        -1 when x < 0
 */
template<typename T>
int sign(T x)
{
  return (x > 0) - (x < 0);
}


//
// Simple Ray structure, just origin & direction
//

struct Ray {
  Vec3f origin;
  Vec3f dir;

  Ray() = default;

  Ray(const Vec3f &o, const Vec3f &d)
    : origin(o)
    , dir(d)
  {
  }
};


//
// Bounding box
//

struct BoundingBox {
  f32 xmin;
  f32 xmax;
  f32 ymin;
  f32 ymax;
  f32 zmin;
  f32 zmax;

  BoundingBox()
    : xmin(1)
    , xmax(-1)
    , ymin(1)
    , ymax(-1)
    , zmin(1)
    , zmax(-1)
  {
  }

  BoundingBox(f32 x0, f32 x1, f32 y0, f32 y1, f32 z0, f32 z1)
    : xmin(x0)
    , xmax(x1)
    , ymin(y0)
    , ymax(y1)
    , zmin(z0)
    , zmax(z1)
  {
  }

  bool is_null() const
  {
    return xmin > xmax;
  }

  void extend(const Vec3f &v);
};

}
