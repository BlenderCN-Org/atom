#include "math.h"

namespace atom {

Vec4f plane_from_vertices(
  const Vec3f &v0,
  const Vec3f &v1,
  const Vec3f &v2)
{
  Vec3f n = cross_product3(v1 - v0, v2 - v0);
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

Vec3f closest_point_on_plane(const Vec3f &point, const Vec4f &plane)
{
  f32 t = (dot_product3(plane, point) - plane.w) / dot_product3(plane, plane);
  // ak je n jednotkovy vektor rovnica sa zjednodusi na
  //f32 t = (dot_product(n, p) - d);
  return point - t * plane.xyz;
}

f32 distance_from_plane(const Vec3f &point, const Vec4f &plane)
{
  return (dot_product3(plane, point) - plane.w) / dot_product3(plane, plane);
}

}
