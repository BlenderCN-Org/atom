#include "intersect.h"

namespace atom {

u32 dominant_axis(const Vec3f &n)
{
  if (abs(n.x) > abs(n.y)) {
    return abs(n.x) > abs(n.z) ? 0 : 2;
  } else {
    return abs(n.y) > abs(n.z) ? 1 : 2;
  }
}

f32 intersect_triangle(const Ray &ray, const Vec3f &v0, const Vec3f &v1, const Vec3f &v2)
{
  const f32 tmin = 0.0001f;
  const f32 tmax = F32_MAX;
  Vec3f b = v2 - v0;
  Vec3f c = v1 - v0;
  Vec3f n = cross_product(c, b);
  // calculate ray plane intersection
  f32 t = -dot_product(ray.origin - v0, n) / dot_product(ray.dir, n);
  // distance test
  if (t <= tmin || t >= tmax) {
    return -1;
  }
  // determine triangle largest 2d projection plane
  u32 k = dominant_axis(n);
  u32 u = (k + 1) % 3;
  u32 v = (k + 2) % 3;
  // calculate hitpoint
  f32 hu = ray.origin[u] + t * ray.dir[u] - v0[u];
  f32 hv = ray.origin[v] + t * ray.dir[v] - v0[v];
  f32 bu = b[u];
  f32 bv = b[v];
  f32 cu = c[u];
  f32 cv = c[v];
  f32 div = bu * cv - bv * cu;
  // calculate & check 2nd barycentric coeficient (1st is 1)
  f32 beta = (bu * hv - bv * hu) / div;
  if (beta < 0) {
    return -1;
  }
  // calculate & check 3rd barycentric coeficient
  f32 gamma = (cv * hu - cu * hv) / div;
  if (gamma < 0 || beta + gamma > 1) {
    return -1;
  }
  // hit
  return t;
}

f32 intersect_triangle_slow(const Ray &ray, const Vec3f &v0, const Vec3f &v1, const Vec3f &v2)
{
  const f32 tmin = 0.0001f;
  Vec3f b = v2 - v0;
  Vec3f c = v1 - v0;

  Vec3f h = cross_product(ray.dir, c);
  f32 a = dot_product(b, h);

  if (a > -tmin && a < tmin) {
    return -1;
  }

  f32 f = 1 / a;

  Vec3f s = ray.origin - v0;

  f32 u = f * dot_product(s, h);

  if (u < 0.0 || u > 1.0) {
    return -1;
  }

  Vec3f q = cross_product(s, b);
  f32 v = f * dot_product(ray.dir, q);

  if (v < 0.0 || u + v > 1.0) {
    return -1;
  }

  f32 t = f * dot_product(c, q);

  if (t > tmin) {
    return t;
  }

  return -1;
}

template<typename IntersectFunc>
f32 intersect_mesh_impl(IntersectFunc intersect_func, const Ray &ray, const Slice<Vec3f> &vertices,
  const Slice<u32> &indices, u32 &index)
{
  assert(indices.size() % 3 == 0);
  f32 tnearest = F32_MAX;
  u32 triangle = U32_MAX;   ///< nearest triangle start index

  for (u32 i = 0; i < indices.size(); i += 3) {
    const Vec3f &v0 = vertices[indices[i    ]];
    const Vec3f &v1 = vertices[indices[i + 1]];
    const Vec3f &v2 = vertices[indices[i + 2]];
    f32 t = intersect_func(ray, v0, v1, v2);

    if (t >= 0 && t < tnearest) {
      triangle = i;
      tnearest = t;
    }
  }

  if (triangle != U32_MAX) {
    index = triangle / 3;
    return tnearest;
  }

  return -1;
}

f32 intersect_mesh(const Ray &ray, const Slice<Vec3f> &vertices,
  const Slice<u32> &indices, u32 &index)
{
  return intersect_mesh_impl(intersect_triangle, ray, vertices, indices, index);
}

f32 intersect_mesh_slow(const Ray &ray, const Slice<Vec3f> &vertices,
                   const Slice<u32> &indices, u32 &index)
{
  return intersect_mesh_impl(intersect_triangle_slow, ray, vertices, indices, index);
}

bool intersect_bounding_box(const Ray &ray, const BoundingBox &box, f32 &tnear, f32 &tfar)
{
  f32 tmin, tmax, tymin, tymax, tzmin, tzmax;
  // division here is used to handle -0, -inf correctly
  f32 divx = 1 / ray.dir.x;
  f32 divy = 1 / ray.dir.y;

  if (divx >= 0) {
    tmin = (box.xmin - ray.origin.x) * divx;
    tmax = (box.xmax - ray.origin.x) * divx;
  } else {
    tmin = (box.xmax - ray.origin.x) * divx;
    tmax = (box.xmin - ray.origin.x) * divx;
  }
  // y
  if (divy >= 0) {
    tymin = (box.ymin - ray.origin.y) * divy;
    tymax = (box.ymax - ray.origin.y) * divy;
  } else {
    tymin = (box.ymax - ray.origin.y) * divy;
    tymax = (box.ymin - ray.origin.y) * divy;
  }

  if ((tmin > tymax) || (tmax < tymin)) {
    return false;
  }
  // z
  f32 divz = 1 / ray.dir.z;

  if (divz >= 0) {
    tzmin = (box.zmin - ray.origin.z) * divz;
    tzmax = (box.zmax - ray.origin.z) * divz;
  } else {
    tzmin = (box.zmax - ray.origin.z) * divz;
    tzmax = (box.zmin - ray.origin.z) * divz;
  }

  tmin = max(tmin, tymin);
  tmax = min(tmax, tymax);

  if ((tmin > tzmax) || (tmax < tzmin)) {
    return false;
  }

  tmin = max(tmin, tzmin);
  tmax = min(tmax, tzmax);

  tnear = tmin;
  tfar = tmax;
  return true;
}

f32 intersect_bounding_box(const Ray &ray, const BoundingBox &box)
{
  f32 tmin, tmax;
  if (!intersect_bounding_box(ray, box, tmin, tmax)) {
    return -1;
  }

  return tmin >= 0 ? tmin : -1;
}

}
