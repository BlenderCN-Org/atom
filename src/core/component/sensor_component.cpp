#include "sensor_component.h"
#include "core/log.h"

namespace atom {

SensorComponent::SensorComponent(Entity &entity)
  : Component(entity)
  , my_body(RigidBodyType::STATIC, entity)
  , my_fixture(Vec2f(1, 1), Vec2f(0, 0), my_body, entity)
{
//  my_fixture.set_filter(1 << 3, (1 << 1) | (1 << 2));
  my_fixture.set_sensor(true);
}

void SensorComponent::set_filter(u16 category, u16 mask)
{
  my_fixture.set_filter(category, mask);
}

void SensorComponent::on_attach()
{

}

void SensorComponent::on_detach()
{

}

void SensorComponent::update()
{
}

bool SensorComponent::is_triggered() const
{
  log::info("%i", my_fixture.collide());
  return my_fixture.collide();
}

}
