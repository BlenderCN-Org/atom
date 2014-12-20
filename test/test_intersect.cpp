#include <core/intersect.h>
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

TEST(IntersectionPlanePlane, ColinearPlanes)
{
  const Vec4f plane_set1[] = {
    Vec4f( 1,  0,  0, 1),
    Vec4f( 0,  1,  0, 1),
    Vec4f( 0,  0,  1, 1),
    Vec4f(-3,  0,  0, 1),
    Vec4f( 0, -3,  0, 1),
    Vec4f( 0,  0, -3, 1),
    Vec4f( 1,  1, -3, 1),
    Vec4f(-5,  4, -3, 2),
    Vec4f( 5,  5, -3, 1),
  };

  const Vec4f plane_set2[] = {
    Vec4f(  1,  0,  0,  2),
    Vec4f(  0,  1,  0,  3),
    Vec4f(  0,  0,  1,  4),
    Vec4f(  4,  0,  0,  2),
    Vec4f(  0,  2,  0,  3),
    Vec4f(  0,  0,  8,  4),
    Vec4f(  2,  2, -6, -1),
    Vec4f(-10,  8, -6,  3),
    Vec4f( 10, 10, -6,  3),
  };

  Ray result;
  int i = 0;
  for (const Vec4f &p1 : plane_set1) {
    const Vec4f &p2 = plane_set2[i];
    ASSERT_FALSE(intersect_plane_plane(p1, p2, result));
    ASSERT_FALSE(intersect_plane_plane(p2, p1, result));
    ++i;
  }
}

TEST(IntersectionPlanePlane, CoincidencePlanes)
{
  const Vec4f plane_set1[] = {
    Vec4f( 1,  0,  0, 0),
    Vec4f( 0,  1,  0, 0),
    Vec4f( 0,  0,  1, 0),
    Vec4f(-3,  0,  0, 2),
    Vec4f( 0, -3,  0, 2),
    Vec4f( 0,  0, -3, 2),
    Vec4f( 1,  1, -3, 3),
    Vec4f(-2,  4, -3, 2),
  };

  const Vec4f plane_set2[] = {
    Vec4f( 1,  0,  0,  0),
    Vec4f( 0,  1,  0,  0),
    Vec4f( 0,  0,  1,  0),
    Vec4f(-6,  0,  0,  1),
    Vec4f( 0, -6,  0,  1),
    Vec4f( 0,  0,  6, -1),
    Vec4f( 3,  3, -9,  1),
    Vec4f(-2,  4, -3, 2),
  };

  Ray result;
  int i = 0;
  for (const Vec4f &p1 : plane_set1) {
    const Vec4f &p2 = plane_set2[i];
    ASSERT_FALSE(intersect_plane_plane(p1, p2, result));
    ASSERT_FALSE(intersect_plane_plane(p2, p1, result));
    ++i;
  }
}

TEST(IntersectionPlanePlane, IntersectingPlanes)
{
  const Vec4f plane_set1[] = {
    Vec4f( 1,  0,  0, 0),
    Vec4f( 1,  0,  0, 0),
    Vec4f( 0,  1,  0, 0),
    Vec4f( 0,  1,  0, 0),
    Vec4f( 0,  0,  1, 0),
    Vec4f( 0,  0,  1, 0),
  };

  const Vec4f plane_set2[] = {
    Vec4f( 0,  1,  0,  0),
    Vec4f( 0,  0,  1,  0),
    Vec4f( 1,  0,  0,  0),
    Vec4f( 0,  0,  1,  0),
    Vec4f( 0,  1,  0,  0),
    Vec4f( 1,  0,  0,  0),
  };

  Ray result;
  int i = 0;
  for (const Vec4f &p1 : plane_set1) {
    const Vec4f &p2 = plane_set2[i];
    ASSERT_TRUE(intersect_plane_plane(p1, p2, result));
    ASSERT_TRUE(intersect_plane_plane(p2, p1, result));
    ++i;
  }
}

TEST(PlaneSign, PositiveSign)
{
  const Vec4f planes[] = {
    Vec4f( 1,  0,  0, 1),
    Vec4f(-1,  0,  0, 1),
    Vec4f( 0,  1,  0, 1),
    Vec4f( 0, -1,  0, 1),
    Vec4f( 0,  0,  1, 1),
    Vec4f( 0,  0, -1, 1)
  };

  const Vec3f points[] = {
    Vec3f( 2,  0,  0),
    Vec3f(-2,  0,  0),
    Vec3f( 0,  2,  0),
    Vec3f( 0, -2,  0),
    Vec3f( 0,  0,  2),
    Vec3f( 0,  0, -2),
  };

  int i = 0;
  for (const Vec4f &plane : planes) {
    const Vec3f &point = points[i];
    ASSERT_GT(plane_sign(point, plane), 0);
    ++i;
  }
}

TEST(PlaneSign, NegativeSign)
{
  const Vec4f planes[] = {
    Vec4f( 1,  0,  0, 2),
    Vec4f(-1,  0,  0, 2),
    Vec4f( 0,  1,  0, 2),
    Vec4f( 0, -1,  0, 2),
    Vec4f( 0,  0,  1, 2),
    Vec4f( 0,  0, -1, 2)
  };

  const Vec3f points[] = {
    Vec3f( 1,  0,  0),
    Vec3f(-1,  0,  0),
    Vec3f( 0,  1,  0),
    Vec3f( 0, -1,  0),
    Vec3f( 0,  0,  1),
    Vec3f( 0,  0, -1),
  };

  int i = 0;
  for (const Vec4f &plane : planes) {
    const Vec3f &point = points[i];
    ASSERT_LT(plane_sign(point, plane), 0);
    ++i;
  }
}

TEST(IntersectLinePlane, Intersection)
{
  const Vec4f planes[] = {
    Vec4f( 1,  0,  0, 1),
    Vec4f(-1,  0,  0, 1),
    Vec4f( 0,  1,  0, 1),
    Vec4f( 0, -1,  0, 1),
    Vec4f( 0,  0,  1, 1),
    Vec4f( 0,  0, -1, 1)
  };

  const Vec3f lines[] = {
    Vec3f( 0,  0,  0),
    Vec3f( 2,  0,  0),

    Vec3f( 0,  0,  0),
    Vec3f(-2,  0,  0),

    Vec3f( 0,  0,  0),
    Vec3f( 0,  2,  0),

    Vec3f( 0,  0,  0),
    Vec3f( 0, -2,  0),

    Vec3f( 0,  0,  0),
    Vec3f( 0,  0,  2),

    Vec3f( 0,  0,  0),
    Vec3f( 0,  0, -2),
  };

  int i = 0;
  for (const Vec4f &plane : planes) {
    const Vec3f &a = lines[i * 2];
    const Vec3f &b = lines[i * 2 + 1];
    Vec3f result;
    ASSERT_TRUE(intersect_line_plane(a, b, plane, result));
    ++i;
  }
}

TEST(IntersectLinePlane, NoIntersection)
{
  const Vec4f planes[] = {
    Vec4f( 1,  0,  0, 1),
    Vec4f(-1,  0,  0, 1),
    Vec4f( 0,  1,  0, 1),
    Vec4f( 0, -1,  0, 1),
    Vec4f( 0,  0,  1, 1),
    Vec4f( 0,  0, -1, 1)
  };

  const Vec3f lines[] = {
    Vec3f( 0,  0,  0),
    Vec3f(-2,  0,  0),

    Vec3f( 0,  0,  0),
    Vec3f( 2,  0,  0),

    Vec3f( 0,  0,  0),
    Vec3f( 0, -2,  0),

    Vec3f( 0,  0,  0),
    Vec3f( 0,  2,  0),

    Vec3f( 0,  0,  0),
    Vec3f( 0,  0, -2),

    Vec3f( 0,  0,  0),
    Vec3f( 0,  0,  2),
  };

  int i = 0;
  for (const Vec4f &plane : planes) {
    const Vec3f &a = lines[i * 2];
    const Vec3f &b = lines[i * 2 + 1];
    Vec3f result;
    ASSERT_FALSE(intersect_line_plane(a, b, plane, result));
    ++i;
  }
}

TEST(IntersectTrianglePlane, Positive)
{
  const Vec4f planes[] = {
    Vec4f( 1,  0,  0, 1),
    Vec4f( 0,  1,  0, 1),
    Vec4f( 0,  0,  1, 1),
  };
  // testuje vsetky if-y
  const Vec3f triangles[] = {
    Vec3f(5, 0,  0), Vec3f(-5, 0, 0), Vec3f(5,  5, 0),
    Vec3f(0, 5,  5), Vec3f( 0, 5, 0), Vec3f(0, -5, 0),
    Vec3f(0, 0, -5), Vec3f( 0, 0, 5), Vec3f(0,  5, 5)
  };

  int i = 0;
  for (const Vec4f &plane : planes) {
    const Vec3f &a = triangles[i * 3];
    const Vec3f &b = triangles[i * 3 + 1];
    const Vec3f &c = triangles[i * 3 + 2];
    Vec3f start;
    Vec3f end;

    ASSERT_TRUE(intersect_triangle_plane(a, b, c, plane, start, end));
    ++i;
  }
}

TEST(IntersectTrianglePlane, Negative)
{
  const Vec4f planes[] = {
    Vec4f( 1,  0,  0, 1),
    Vec4f( 0,  1,  0, 1),
    Vec4f( 0,  0,  1, 1),
  };

  const Vec3f triangles[] = {
    Vec3f(-5,  0,  0), Vec3f(0, 0, 1), Vec3f(0, 0, 2),
    Vec3f( 0, -5,  0), Vec3f(1, 0, 0), Vec3f(2, 0, 0),
    Vec3f( 0,  0, -5), Vec3f(0, 1, 0), Vec3f(0, 2, 0),
  };

  int i = 0;
  for (const Vec4f &plane : planes) {
    const Vec3f &a = triangles[i * 3];
    const Vec3f &b = triangles[i * 3 + 1];
    const Vec3f &c = triangles[i * 3 + 2];
    Vec3f start;
    Vec3f end;

    ASSERT_FALSE(intersect_triangle_plane(a, b, c, plane, start, end));
    ++i;
  }
}

TEST(IntersectCircleTriangle, Positive)
{
  const Circle circles[] = {
    Circle(Vec3f(0, 0, 0), Vec3f(1, 0, 0), 1)
  };

  const Vec3f triangles[] = {
    Vec3f(0, 0, 0), Vec3f(1, 5, 0), Vec3f(-1, 5, 0)
  };

  int i = 0;
  for (const Circle &circle : circles) {
    const Vec3f &a = triangles[i * 3];
    const Vec3f &b = triangles[i * 3 + 1];
    const Vec3f &c = triangles[i * 3 + 2];

    ASSERT_TRUE(intersect_circle_triangle(circle, a, b, c));
  }

}

}
