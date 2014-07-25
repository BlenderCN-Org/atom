#include "math.h"

namespace atom {

Vec4f plane_from_vertices(
  const Vec3f &v0,
  const Vec3f &v1,
  const Vec3f &v2)
{
  Vec3f n = cross_product(v1 - v0, v2 - v0);
  float    d = - (n[0] * v0[0] + n[1] * v0[1] * n[2] * v0[2]);
  return Vec4f(n[0], n[1], n[2], d);
}

}
