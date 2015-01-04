#include "math.h"

namespace atom {

Vec4f plane_from_vertices(
  const Vec3f &v0,
  const Vec3f &v1,
  const Vec3f &v2)
{
  Vec3f n = cross3(v1 - v0, v2 - v0);
  float    d = - (n[0] * v0[0] + n[1] * v0[1] * n[2] * v0[2]);
  return Vec4f(n[0], n[1], n[2], d);
}

void BoundingBox::extend(const Vec3f &v)
{
  xmin = min(v.x, xmin);
  xmax = max(v.x, xmax);
  ymin = min(v.y, ymin);
  ymax = max(v.y, ymax);
  zmin = min(v.z, zmin);
  zmax = max(v.z, zmax);
}

}
