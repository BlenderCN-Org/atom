#include "b2_utils.h"
#include "core/component/fixture_component.h"

namespace atom {

NearestRayCast::NearestRayCast(u16 mask)
  : my_nearest(nullptr)
  , my_mask(mask)
{

}

FixtureComponent *NearestRayCast::nearest_fixture() const
{
  return my_nearest;
}

f32 NearestRayCast::nearest_fraction() const
{
  return my_fraction;
}

float32 NearestRayCast::ReportFixture(b2Fixture *fixture, const b2Vec2 &point,
  const b2Vec2 &normal, float32 fraction)
{
  if (fixture->GetFilterData().categoryBits & my_mask) {
    my_fraction = fraction;
    my_nearest = reinterpret_cast<FixtureComponent *>(fixture->GetUserData());
    return fraction;
  } else {
    return -1;
  }

//  return 1;
}

}
