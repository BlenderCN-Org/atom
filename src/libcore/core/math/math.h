#pragma once

#include "../platform.h"
#include "../meta.h"
#include "../slice.h"
#include "vec.h"
#include "mat.h"
#include "quat.h"

namespace atom {

//
// Vector specialization for most useful types
//
typedef Vec2<u32> Vec2u32;
typedef Vec4<u8> Vec4u8;
typedef Vec2<float> Vec2f;
typedef Vec3<float> Vec3f;
typedef Vec4<float> Vec4f;

static_assert(sizeof(Vec2u32) == 8, "Velkost musi byt 8bytov");
static_assert(sizeof(Vec2f) == 8, "Velkost musi byt 8bytov");
static_assert(sizeof(Vec3f) == 12, "Velkost musi byt 8bytov");
static_assert(sizeof(Vec4f) == 16, "Velkost musi byt 8bytov");

MAP_TYPE(Vec2f, VEC2F)
MAP_TYPE(Vec3f, VEC3F)
MAP_TYPE(Vec4f, VEC4F)


//
// Matrix specialization for most useful types
//

typedef Mat2<float> Mat2f;
typedef Mat3<float> Mat3f;
typedef Mat4<float> Mat4f;

MAP_TYPE(Mat2f, MAT2F)
MAP_TYPE(Mat3f, MAT3F)
MAP_TYPE(Mat4f, MAT4F)


//
// Quaternion using float (quaternions using double precision aren't used).
//

typedef Quat<f32> Quatf;
static_assert(sizeof(Quatf) == 16, "Size of the Quaterion<float> should be 16 bytes");


/**
 * Multiply Mat4 (using only submatrix 3x3) and Vec3.
 * It is equivalent of m * Vec4(v, 0).
 */
template<typename T>
Vec3<T> rotate(const Mat4<T> &m, const Vec3<T> &v)
{
  return Vec3<T>(
    m(0, 0) * v.x + m(0, 1) * v.y + m(0, 2) * v.z,
    m(1, 0) * v.x + m(1, 1) * v.y + m(1, 2) * v.z,
    m(2, 0) * v.x + m(2, 1) * v.y + m(2, 2) * v.z);
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

struct Ray {
  Vec3f origin;
  Vec3f dir;
};

/**
 * Calculate nearest intersection of ray and triangle mesh.
 */
f32 intersect(const Slice<Vec3f> &vertices, const Slice<u32> &indices,
  const Ray &ray, u32 &index);

}
