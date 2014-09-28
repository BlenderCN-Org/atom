#pragma once

#include "math.h"

namespace atom {

u32 dominant_axis(const Vec3f &n);

/**
 * Fast intersection between ray and triangle (Ingo Wald projection method).
 *
 * @return positive value means hit, negative value means miss, nan is returned
 *         when ray lies in the triangle plane (infinite number of intersections)
 */
f32 intersect_triangle(const Ray &ray, const Vec3f &v0, const Vec3f &v1, const Vec3f &v2);

/**
 * Intersection between ray and triangle using Moller-Trumbore algorithm.
 */
f32 intersect_triangle_slow(const Ray &ray, const Vec3f &v0, const Vec3f &v1, const Vec3f &v2);

/**
 * Calculate nearest intersection of ray and triangle mesh.
 */
f32 intersect_mesh(const Ray &ray, const Slice<Vec3f> &vertices,
  const Slice<u32> &indices, u32 &index);

/**
 * Nearest intersection of ray and triangle mesh using intersect_mesh_slow.
 */
f32 intersect_mesh_slow(const Ray &ray, const Slice<Vec3f> &vertices,
  const Slice<u32> &indices, u32 &index);

/**
 * Get intersection of a line (infinite endpoints) and box.
 * @param[out] tnear nearest intersect point (on intersect)
 * @param[out] tfar
 * @return true when line intersects the box
 */
bool intersect_bounding_box(const Ray &ray, const BoundingBox &box, f32 &tnear, f32 &tfar);

/**
 * Get neareset intersection. Ray starts at origin (t=0) and extends to infinity.
 *
 * @return >=0 nearest intersection, negative value means no intersection
 */
f32 intersect_bounding_box(const Ray &ray, const BoundingBox &box);

}
