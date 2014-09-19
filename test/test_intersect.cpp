#include <core/math/math.h>
#include <gtest/gtest.h>

namespace atom {

TEST(RayBoxIntersect, DoubleHit)
{
  Ray hit(Vec3f(0, 0, -5), Vec3f(0, 0, 1));
  BoundingBox box(-1, 1, -1, 1, -1, 1);
  f32 tmin = -1;
  f32 tmax = -1;
  ASSERT_TRUE(intersect(hit, box, tmin, tmax));
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
  ASSERT_FLOAT_EQ(1.0f, intersect(hitx, box));
  ASSERT_FLOAT_EQ(1.0f, intersect(hity, box));
  ASSERT_FLOAT_EQ(1.0f, intersect(hitz, box));
  // inverted x ray
  hitx.origin = -hitx.origin;
  hitx.dir = -hitx.dir;
  ASSERT_FLOAT_EQ(1.0f, intersect(hitx, box));
  // inverted y ray
  hity.origin = -hity.origin;
  hity.dir = -hity.dir;
  ASSERT_FLOAT_EQ(1.0f, intersect(hity, box));
  // inverted z ray
  hitz.origin = -hitz.origin;
  hitz.dir = -hitz.dir;
  ASSERT_FLOAT_EQ(1.0f, intersect(hitz, box));
}

TEST(RayBoxNearestIntersect, HitEdges)
{
  BoundingBox box(-1, 1, -1, 1, -1, 1);
  Ray hitx(Vec3f(-2, 1,  0), Vec3f(1, 0, 0));
  Ray hity(Vec3f(1, -2,  0), Vec3f(0, 1, 0));
  Ray hitz(Vec3f(0,  1, -2), Vec3f(0, 0, 1));
  ASSERT_FLOAT_EQ(1.0f, intersect(hitx, box));
  ASSERT_FLOAT_EQ(1.0f, intersect(hity, box));
  ASSERT_FLOAT_EQ(1.0f, intersect(hitz, box));
  // inverted x ray
  hitx.origin = -hitx.origin;
  hitx.dir = -hitx.dir;
  ASSERT_FLOAT_EQ(1.0f, intersect(hitx, box));
  // inverted y ray
  hity.origin = -hity.origin;
  hity.dir = -hity.dir;
  ASSERT_FLOAT_EQ(1.0f, intersect(hity, box));
  // inverted z ray
  hitz.origin = -hitz.origin;
  hitz.dir = -hitz.dir;
  ASSERT_FLOAT_EQ(1.0f, intersect(hitz, box));
}

TEST(RayBoxNearestIntersect, Miss)
{
  BoundingBox box(-1, 1, -1, 1, -1, 1);
  Ray hitx(Vec3f(-2, 2,  0), Vec3f(1, 0, 0));
  Ray hity(Vec3f(2, -2,  0), Vec3f(0, 1, 0));
  Ray hitz(Vec3f(0,  2, -2), Vec3f(0, 0, 1));
  ASSERT_GT(0.0f, intersect(hitx, box));
  ASSERT_GT(1.0f, intersect(hity, box));
  ASSERT_GT(1.0f, intersect(hitz, box));
  // inverted x ray
  hitx.origin = -hitx.origin;
  hitx.dir = -hitx.dir;
  ASSERT_GT(0.0f, intersect(hitx, box));
  // inverted y ray
  hity.origin = -hity.origin;
  hity.dir = -hity.dir;
  ASSERT_GT(1.0f, intersect(hity, box));
  // inverted z ray
  hitz.origin = -hitz.origin;
  hitz.dir = -hitz.dir;
  ASSERT_GT(1.0f, intersect(hitz, box));
}

}
