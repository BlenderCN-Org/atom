#include <gtest/gtest.h>
#include <core/math/math.h>
#include <core/log.h>
#include <core/utils/utils.h>
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

}
