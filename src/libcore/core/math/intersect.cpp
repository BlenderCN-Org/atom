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
  Vec3f n = cross_product3(c, b);
  // calculate ray plane intersection
  f32 t = -dot_product3(ray.origin - v0, n) / dot_product3(ray.dir, n);
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

  Vec3f h = cross_product3(ray.dir, c);
  f32 a = dot_product3(b, h);

  if (a > -tmin && a < tmin) {
    return -1;
  }

  f32 f = 1 / a;

  Vec3f s = ray.origin - v0;

  f32 u = f * dot_product3(s, h);

  if (u < 0.0 || u > 1.0) {
    return -1;
  }

  Vec3f q = cross_product3(s, b);
  f32 v = f * dot_product3(ray.dir, q);

  if (v < 0.0 || u + v > 1.0) {
    return -1;
  }

  f32 t = f * dot_product3(c, q);

  if (t > tmin) {
    return t;
  }

  return -1;
}

template<typename IntersectFunc>
f32 intersect_mesh_impl(IntersectFunc intersect_func, const Ray &ray,
  const Slice<Vec3f> &vertices, const Slice<u32> &indices, u32 &index)
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

bool intersect_plane(const Vec3f &a, const Vec3f &b, const Vec4f &plane)
{
  const Vec3f ab = b - a;
  const f32 t = (plane.w - dot_product3(plane.xyz(), a)) / dot_product3(plane.xyz(), ab);
  
  if (t >= 0.0f && t <= 1.0f) {
    Vec3f q = a + t * ab;
    return true;
  }
  // no intersection
  return false;
}

Vec3f closest_point_on_plane(const Vec3f &point, const Vec4f &plane)
{
  f32 t = (dot_product3(plane, point) - plane.w) / dot_product3(plane, plane);
  // ak je n jednotkovy vektor rovnica sa zjednodusi na
  //f32 t = (dot_product(n, p) - d);
  return point - t * plane.xyz();
}

f32 distance_from_plane(const Vec3f &point, const Vec4f &plane)
{
  return (dot_product3(plane, point) - plane.w) / dot_product3(plane, plane);
}

f32 closest_point_on_line_segment(const Vec3f &point, const Vec3f &a,
  const Vec3f &b, Vec3f &result)
{
  const Vec3f ab = b - a;
  // project point onto ab
  f32 t = dot_product3(point - a, ab) / dot_product3(ab, ab);
  // if outside segment, clamp t to the closest endpoint
  if (t < 0.0f) {
    t = 0.0f;
  } else if (t > 1.0f) {
    t = 1.0f;
  }
  // compute result
  result = a + t * ab;
  return t;
}

f32 distance_from_segment2(const Vec3f &point, const Vec3f &a, const Vec3f &b)
{
  const Vec3f ab = b - a;
  const Vec3f ac = point - a;
  const Vec3f bc = point - b;
  const f32 e = dot_product3(ac, ab);
  // when point projects outside ab
  if (e <= 0.0f) {
    return dot_product3(ac, ac);
  }
  const f32 f = dot_product3(ab, ab);
  if (e >= f) {
    return dot_product3(bc, bc);
  }
  // when point projects on ab
  return dot_product3(ac, ac) - e * e / f;
}

f32 distance_from_segment(const Vec3f &point, const Vec3f &a, const Vec3f &b)
{
  return std::sqrt(distance_from_segment2(point, a, b));
}

Vec3f closest_point_on_triangle(const Vec3f &point, const Vec3f &a, const Vec3f &b, const Vec3f &c)
{
  const Vec3f ab = b - a;
  const Vec3f ac = c - a;
  const Vec3f bc = c - b;
  const Vec3f ap = point - a;
  const Vec3f bp = point - b;
  
  const f32 snom = dot_product3(ap, ab);
  const f32 sdenom = dot_product3(bp, a - b);
  
  const f32 tnom = dot_product3(ap, ac);
  const f32 tdenom = dot_product3(point - c, a - c);
  
  if (snom <= 0.0f && tnom <= 0.0f) {
    return a;
  }
  
  const f32 unom = dot_product3(bp, bc);
  const f32 udenom = dot_product3(point - c, b - c);
  
  if (sdenom <= 0.0f && unom <= 0.0f) {
    return b;
  }
  if (tdenom <= 0.0f && udenom <= 0.0f) {
    return c;
  }
  
  const Vec3f n = cross_product3(ab, ac);
  const f32 vc = dot_product3(n, cross_product3(a - point, b - point));
  
  if (vc <= 0.0f && snom >=0.0f && sdenom >= 0.0f) {
    return a + snom / (snom + sdenom) * ab;
  }
  
  const f32 va = dot_product3(n, cross_product3(b - point, c - point));
  if (va <= 0.0f && unom >= 0.0f && udenom >= 0.0f) {
    return b + unom / (unom + udenom) * bc;
  }
  
  const f32 vb = dot_product3(n, cross_product3(c - point, a - point));
  if (vb <= 0.0f && tnom >= 0.0f && tdenom >= 0.0f) {
    return c + tnom / (tnom + tdenom) * ac;
  }
  // project inside triangle
  const f32 u = va / (va + vb + vc);
  const f32 v = vb / (va + vb + vc);
  const f32 w = 1.0f - u - v;
  return u * a + v * b * w * c;
}

bool intersect_plane_plane(const Vec4f &p1, const Vec4f &p2, Ray &result)
{
  // calculate direction of intersection line
  const Vec3f d = cross_product3(p1.xyz(), p2.xyz());
  // when d is close to zero, the planes are parallel or coincident
  f32 denom = dot_product3(d, d);
  if (denom < EPSILON) {
    return false;
  }
  // compute point on intersection line
  result.origin = cross_product3(p1.w * p2.xyz() - p2.w * p1.xyz(), d) / denom;
  result.dir = d;
  return true;
}

}
