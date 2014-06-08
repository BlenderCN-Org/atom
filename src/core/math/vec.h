#pragma once

#include "core/math/vec2.h"
#include "core/math/vec3.h"
#include "core/math/vec4.h"

namespace atom {

/**
 * 2D vector/point
 * 2-component vector (x, y, z, w);
 */

typedef Vec2<uint32_t> Vec2u;
static_assert(sizeof(Vec2u) == 8, "Velkost musi byt 8bytov");

typedef Vec2<float> Vec2f;
static_assert(sizeof(Vec2f) == 8, "Velkost musi byt 8bytov");

template<>
inline Type type_of<Vec2f>()
{
  return Type::VEC2F;
}

/**
 * 3D vector/point
 * 3-component vector (x, y, z, w);
 */
typedef Vec3<float> Vec3f;
static_assert(sizeof(Vec3f) == 12, "Velkost musi byt 8bytov");

template<>
inline Type type_of<Vec3f>()
{
  return Type::VEC3F;
}

/**
 * 4D vector/point
 * 4-component vector (x, y, z, w);
 */
typedef Vec4<float> Vec4f;
static_assert(sizeof(Vec4f) == 16, "Velkost musi byt 8bytov");

template<>
inline Type type_of<Vec4f>()
{
  return Type::VEC4F;
}

/**
 * Construct Vec3 from Vectro4 using x,y,z values.
 */
template<typename T>
Vec3<T> to_vec3(const Vec4<T> &v)
{
  return Vec3<T>(v.x, v.y, v.z);
}

/**
 * Calculate 3d point position from 4d vector.
 */
template<typename T>
Vec3<T> to_point3(const Vec4<T> &v)
{
  return Vec3<T>(v.x / v.w, v.y / v.w, v.z / v.w);
}

}
