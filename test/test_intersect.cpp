#include <core/math/intersect.h>
#include <gtest/gtest.h>
#include <core/log.h>

namespace atom {

TEST(RayBoxIntersect, DoubleHit)
{
  Ray hit(Vec3f(0, 0, -5), Vec3f(0, 0, 1));
  BoundingBox box(-1, 1, -1, 1, -1, 1);
  f32 tmin = -1;
  f32 tmax = -1;
  ASSERT_TRUE(intersect_bounding_box(hit, box, tmin, tmax));
  ASSERT_FLOAT_EQ(4.0f, tmin);
  ASSERT_FLOAT_EQ(6.0f, tmax);
}

// nearest intersection
TEST(RayBoxNearestIntersect, Hit)
{
  BoundingBox box(-1, 1, -1, 1, -1, 1);
  Ray hitx(Vec3f(-2, 0,  0), Vec3f(1, 0, 0));
  Ray hity(Vec3f(0, -2,  0), Vec3f(0, 1, 0));
  Ray hitz(Vec3f(0,  0, -2), Vec3f(0, 0, 1));
  ASSERT_FLOAT_EQ(1.0f, intersect_bounding_box(hitx, box));
  ASSERT_FLOAT_EQ(1.0f, intersect_bounding_box(hity, box));
  ASSERT_FLOAT_EQ(1.0f, intersect_bounding_box(hitz, box));
  // inverted x ray
  hitx.origin = -hitx.origin;
  hitx.dir = -hitx.dir;
  ASSERT_FLOAT_EQ(1.0f, intersect_bounding_box(hitx, box));
  // inverted y ray
  hity.origin = -hity.origin;
  hity.dir = -hity.dir;
  ASSERT_FLOAT_EQ(1.0f, intersect_bounding_box(hity, box));
  // inverted z ray
  hitz.origin = -hitz.origin;
  hitz.dir = -hitz.dir;
  ASSERT_FLOAT_EQ(1.0f, intersect_bounding_box(hitz, box));
}

TEST(RayBoxNearestIntersect, HitEdges)
{
  BoundingBox box(-1, 1, -1, 1, -1, 1);
  Ray hitx(Vec3f(-2, 1,  0), Vec3f(1, 0, 0));
  Ray hity(Vec3f(1, -2,  0), Vec3f(0, 1, 0));
  Ray hitz(Vec3f(0,  1, -2), Vec3f(0, 0, 1));
  ASSERT_FLOAT_EQ(1.0f, intersect_bounding_box(hitx, box));
  ASSERT_FLOAT_EQ(1.0f, intersect_bounding_box(hity, box));
  ASSERT_FLOAT_EQ(1.0f, intersect_bounding_box(hitz, box));
  // inverted x ray
  hitx.origin = -hitx.origin;
  hitx.dir = -hitx.dir;
  ASSERT_FLOAT_EQ(1.0f, intersect_bounding_box(hitx, box));
  // inverted y ray
  hity.origin = -hity.origin;
  hity.dir = -hity.dir;
  ASSERT_FLOAT_EQ(1.0f, intersect_bounding_box(hity, box));
  // inverted z ray
  hitz.origin = -hitz.origin;
  hitz.dir = -hitz.dir;
  ASSERT_FLOAT_EQ(1.0f, intersect_bounding_box(hitz, box));
}

TEST(RayBoxNearestIntersect, Miss)
{
  BoundingBox box(-1, 1, -1, 1, -1, 1);
  Ray hitx(Vec3f(-2, 2,  0), Vec3f(1, 0, 0));
  Ray hity(Vec3f(2, -2,  0), Vec3f(0, 1, 0));
  Ray hitz(Vec3f(0,  2, -2), Vec3f(0, 0, 1));
  ASSERT_GT(0.0f, intersect_bounding_box(hitx, box));
  ASSERT_GT(1.0f, intersect_bounding_box(hity, box));
  ASSERT_GT(1.0f, intersect_bounding_box(hitz, box));
  // inverted x ray
  hitx.origin = -hitx.origin;
  hitx.dir = -hitx.dir;
  ASSERT_GT(0.0f, intersect_bounding_box(hitx, box));
  // inverted y ray
  hity.origin = -hity.origin;
  hity.dir = -hity.dir;
  ASSERT_GT(1.0f, intersect_bounding_box(hity, box));
  // inverted z ray
  hitz.origin = -hitz.origin;
  hitz.dir = -hitz.dir;
  ASSERT_GT(1.0f, intersect_bounding_box(hitz, box));
}

TEST(RayBoxNearestIntersect, MissButOriginInside)
{
  BoundingBox box(-1, 1, -1, 1, -1, 1);
  Ray miss(Vec3f(0, 0,  0), Vec3f(2, 0, 0));
  ASSERT_GT(0.0f, intersect_bounding_box(miss, box));
}

TEST(RayTriangleIntersect, Hit)
{
  Ray hit(Vec3f(0.0f, -1.0f, 0.0f), Vec3f(0, 1, 0));
  ASSERT_FLOAT_EQ(1.0f, intersect_triangle(hit, Vec3f(-1, 0, -1), Vec3f(1, 0, -1), Vec3f(0, 0, 1)));
  // ray lies in the triangle, nan is returned
  ASSERT_TRUE(isnan(intersect_triangle(hit, Vec3f(-1, -1, 0), Vec3f(1, -1, 0), Vec3f(0, 1, 0))));
}


TEST(RayTriangleIntersect, Hit2)
{
//  Ray hit(Vec3f(-0.9, -1.0f, 0.1), Vec3f(0, 1, 0));
//  ASSERT_EQ(1.0f, intersect_triangle(hit, Vec3f(-1, 0, 1), Vec3f(1, 0, 1), Vec3f(1, 0, 3)));
  Ray hit(Vec3f(0.5f, -2.0f, -0.1f), Vec3f(0, 1, 0));
  const Vec3f v0(-1, 0, -1);
  const Vec3f v1(1, 0, -1);
  const Vec3f v2(1, 0, 1);
  ASSERT_EQ(2.0f, intersect_triangle(hit, v0, v1, v2));
  ASSERT_EQ(2.0f, intersect_triangle_slow(hit, v0, v1, v2));
}

TEST(RayTriangleIntersect, Miss)
{
  Ray hit(Vec3f(0.0f, -1.0f, 0.0f), Vec3f(0, -1, 0));
  ASSERT_GT(0.0f, intersect_triangle(hit, Vec3f(-1, 0, -1), Vec3f(1, 0, -1), Vec3f(0, 0, 1)));
}

TEST(RayMeshIntersect, Hit)
{
  const Vec3f points[] = {
    Vec3f(-1, 0, -1),
    Vec3f( 1, 0, -1),
    Vec3f( 1, 0,  1),
    Vec3f(-1, 0,  1)
  };

  const u32 indices[] = { 0, 1, 2, 0, 2, 3 };

  Ray hit0(Vec3f( 0.5f, -2.0f, -0.5f), Vec3f(0, 1, 0));
  Ray hit1(Vec3f(-0.5f, -2.0f,  0.5f), Vec3f(0, 1, 0));
  u32 triangle = U32_MAX;

  ASSERT_EQ(2.0f, intersect_mesh(hit0, Slice<Vec3f>(points, 4),
    Slice<u32>(indices, 6), triangle));
  ASSERT_EQ(0, triangle);

  ASSERT_EQ(2.0f, intersect_mesh(hit1, Slice<Vec3f>(points, 4),
    Slice<u32>(indices, 6), triangle));
  ASSERT_EQ(1, triangle);
}

TEST(RayMeshIntersect, Box)
{
  // cube vertices
  const f32 raw_vertices[24] = {
     1,  1, -1,
     1, -1, -1,
    -1, -1, -1,
    -1,  1, -1,
     1,  1,  1,
     1, -1,  1,
    -1, -1,  1,
    -1,  1,  1 };
  // cube indices
  const u32 raw_indices[36] = {
    1,2,3,
    7,6,5,
    0,4,5,
    1,5,6,
    6,7,3,
    0,3,7,
    0,1,3,
    4,7,5,
    1,0,5,
    2,1,6,
    2,6,3,
    4,0,7
  };

  const Slice<Vec3f> vertices(reinterpret_cast<const Vec3f *>(raw_vertices), 8);
  const Slice<u32> indices(raw_indices, 36);
  // set of test rays
  const Ray test_rays[] = {
    // internal rays
    Ray(Vec3f( 0,  0,  0), Vec3f( 1,  0,  0)),
    Ray(Vec3f( 0,  0,  0), Vec3f( 0,  1,  0)),
    Ray(Vec3f( 0,  0,  0), Vec3f( 0,  0,  1)),
    Ray(Vec3f( 0,  0,  0), Vec3f(-1,  0,  0)),
    Ray(Vec3f( 0,  0,  0), Vec3f( 0, -1,  0)),
    Ray(Vec3f( 0,  0,  0), Vec3f( 0,  0, -1)),
    // externat x-rays
    Ray(Vec3f( 2,  0,  0), Vec3f( 1,  0,  0)),
    Ray(Vec3f( 2,  0,  0), Vec3f(-1,  0,  0)),
    Ray(Vec3f(-2,  0,  0), Vec3f( 1,  0,  0)),
    Ray(Vec3f(-2,  0,  0), Vec3f(-1,  0,  0)),
    // external y-rays
    Ray(Vec3f( 0,  2,  0), Vec3f( 0,  1,  0)),
    Ray(Vec3f( 0,  2,  0), Vec3f( 0, -1,  0)),
    Ray(Vec3f( 0, -2,  0), Vec3f( 0,  1,  0)),
    Ray(Vec3f( 0, -2,  0), Vec3f( 0, -1,  0)),
    // external z-rays
    Ray(Vec3f( 0,  0,  2), Vec3f( 0,  0,  1)),
    Ray(Vec3f( 0,  0,  2), Vec3f( 0,  0, -1)),
    Ray(Vec3f( 0,  0, -2), Vec3f( 0,  0,  1)),
    Ray(Vec3f( 0,  0, -2), Vec3f( 0,  0, -1)),
  };
  // compare two methods, results has to be equal
  for (const Ray &ray : test_rays) {
    u32 i, j;
    // returned t should be the same
    ASSERT_FLOAT_EQ(intersect_mesh(ray, vertices, indices, i),
      intersect_mesh_slow(ray, vertices, indices, j));
    // compare value of returned/colliding triangle index
    ASSERT_EQ(i, j);
  }
}

}
