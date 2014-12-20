#include <gtest/gtest.h>
#include <core/intersect.h>
#include <core/log.h>
#include <core/utils.h>
#include "test_utils.h"

namespace atom {

TEST(ClosestPointOnPlane, PlaneXY)
{
  const Vec4f plane(0, 0, 1, 3);

  const Vec3f points[] = {
    Vec3f( 0,  0,  4),
    Vec3f( 0,  1,  3),
    Vec3f( 1,  0,  3),
    Vec3f( 0,  0, -1),
    Vec3f( 0, -1,  3),
    Vec3f(-1,  0,  3),
  };

  const Vec3f results[] = {
    Vec3f( 0,  0, 3),
    Vec3f( 0,  1, 3),
    Vec3f( 1,  0, 3),
    Vec3f( 0,  0, 3),
    Vec3f( 0, -1, 3),
    Vec3f(-1,  0, 3),
  };

  int i = 0;
  for (const Vec3f &p : points) {
    const Vec3f c = closest_point_on_plane(p, plane);
    const Vec3f result = results[i];
    EXPECT_VEC3F_NEAR(result, c, EPSILON);
    ++i;
  }
}

TEST(DistanceFromPlane, PrimitiveTest)
{
  const Vec4f planes[] = {
    Vec4f(0, 0, 1, 0),
    Vec4f(0, 0, 1, 0),
    Vec4f(0, 0, 1, 0),
  };

  const Vec3f points[] = {
    Vec3f(0, 0, 1),
    Vec3f(0, 0, 1),
    Vec3f(0, 0, 1),
  };

  const f32 expected_results[] = {
    1,
    1,
    1
  };

  int i = 0;
  for (const Vec4f &plane : planes) {
    const Vec3f &point = points[i];
    EXPECT_NEAR(expected_results[i], distance_from_plane(point, plane), EPSILON);
    ++i;
  }
}

TEST(ClosestPointOnLineSegment, Test)
{
  const Vec3f segments[][2] = {
    { Vec3f( 5,  0, 0), Vec3f(15,  0, 0) },
    { Vec3f( 5,  0, 0), Vec3f(15,  0, 0) },
    { Vec3f( 5,  0, 0), Vec3f(15,  0, 0) },
    { Vec3f( 5,  0, 0), Vec3f(15,  0, 0) },
    { Vec3f( 5,  0, 0), Vec3f(15,  0, 0) },
    { Vec3f(-5, -8, 0), Vec3f( 5, 20, 0) },
    { Vec3f(-5, -8, 0), Vec3f( 5, 20, 0) },
  };

  const Vec3f points[] = {
    Vec3f( 0,  0, 0), // < start
    Vec3f( 5,  0, 0), // start
    Vec3f(10,  0, 0), // center
    Vec3f(15,  0, 0), // end
    Vec3f(20,  0, 0), // > end
    Vec3f(-9, -9, 5), // < start
    Vec3f( 0,  6, 2), // center
  };

  const Vec3f expected_points[] = {
    Vec3f( 5,  0, 0),
    Vec3f( 5,  0, 0),
    Vec3f(10,  0, 0),
    Vec3f(15,  0, 0),
    Vec3f(15,  0, 0),
    Vec3f(-5, -8, 0),
    Vec3f( 0,  6, 0),
  };

  const f32 expected_t[] = {
    0,
    0,
    0.5f,
    1,
    1,
    0,
    0.5,
  };

  int i = 0;
  for (const Vec3f *segment : segments) {
    const Vec3f &point = points[i];
    const Vec3f &a = segment[0];
    const Vec3f &b = segment[1];
    Vec3f result;
    EXPECT_NEAR(expected_t[i], closest_point_on_line_segment(point, a, b, result), EPSILON);
    EXPECT_VEC3F_NEAR(expected_points[i], result, EPSILON);
    ++i;
  }
}

TEST(SquaredDistanceFromLineSegment, Test)
{
  const Vec3f segments[][2] = {
    { Vec3f( -10, 5, 5 ), Vec3f(-4, 5, 5) },
    { Vec3f( -10, 5, 5 ), Vec3f(-4, 5, 5) },
    { Vec3f( -10, 5, 5 ), Vec3f(-4, 5, 5) },
    { Vec3f( -10, 5, 5 ), Vec3f(-4, 5, 5) },
    { Vec3f( -10, 5, 5 ), Vec3f(-4, 5, 5) },
  };

  const Vec3f points[] = {
    Vec3f(-11, 5, 5),   // < start
    Vec3f(-10, 5, 5),   // start
    Vec3f( -7, 5, 5),   // center
    Vec3f( -4, 5, 5),   // end
    Vec3f( -4, 7, 5),   // > end
  };

  const f32 results[] = {
    1,
    0,
    0,
    0,
    2,
  };

  int i = 0;
  for (const Vec3f *segment : segments) {
    const Vec3f &a = segment[0];
    const Vec3f &b = segment[1];
    const Vec3f &point = points[i];

    EXPECT_NEAR(results[i], distance_from_segment(point, a, b), EPSILON);
    ++i;
  }
}

TEST(Sign, Negative)
{
  ASSERT_EQ(-1, sign(-1));
  ASSERT_EQ(-1, sign(-5));
  ASSERT_EQ(-1, sign(-123));
}

TEST(Sign, Positive)
{
  ASSERT_EQ(1, sign(1));
  ASSERT_EQ(1, sign(5));
  ASSERT_EQ(1, sign(123));
}
TEST(Sign, Zero)
{
  ASSERT_EQ(0, sign(0));
}



}
