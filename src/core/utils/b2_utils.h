#pragma once

#include <Box2D/Box2D.h>
#include "core/math/vec.h"
#include "core/corefwd.h"
#include "core/noncopyable.h"

namespace atom {

inline b2Vec2 to_b2_vec2(const Vec2f &v)
{
  return b2Vec2(v.x, v.y);
}

inline Vec2f to_vec2f(const b2Vec2 &v)
{
  return Vec2f(v.x, v.y);
}

class NearestRayCast : public b2RayCastCallback {
public:
  explicit NearestRayCast(u16 mask = 0xFFFF);

  FixtureComponent* nearest_fixture() const;

  f32 nearest_fraction() const;

  float32 ReportFixture (b2Fixture *fixture, const b2Vec2 &point, const b2Vec2 &normal, float32 fraction);

private:
  f32 my_fraction;
  FixtureComponent *my_nearest;
  u16 my_mask;
};

}
