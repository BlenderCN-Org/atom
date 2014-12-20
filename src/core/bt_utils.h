#pragma once

#include "foundation.h"
#include <bullet/btBulletCollisionCommon.h>

namespace atom {

inline btVector3 to_bt_vector3(const Vec3f &v)
{
  return btVector3(v.x, v.y, v.z);
}

inline Vec3f to_vec3(const btVector3 &v)
{
  return Vec3f(v.x(), v.y(), v.z());
}

inline btQuaternion to_bt_quaterion(const Quatf &q)
{
  return btQuaternion(q.x, q.y, q.z, q.w);
}

}
