#include "fixture_component.h"
#include "core/world/world.h"
#include "core/processor/physics_processor.h"
#include "core/component/rigid_body_component.h"

namespace atom {

FixtureComponent::FixtureComponent(RigidBodyComponent &body, Entity &entity)
  : Component(entity)
  , my_fixture(nullptr)
//  , my_density(1)
//  , my_friction(0.1)
//  , my_is_sensor(false)
//  , my_category(1)
//  , my_mask(0xFFFF)
  , my_body(body)
{

}

void FixtureComponent::on_attach()
{
  processors().physics.add_fixture(this);
}

void FixtureComponent::on_detach()
{
  processors().physics.remove_fixture(this);
}

bool FixtureComponent::is_sensor() const
{
  return my_fixture->IsSensor();
}

void FixtureComponent::set_sensor(bool sensor)
{
  my_fixture->SetSensor(sensor);
}

u16 FixtureComponent::category() const
{
  return my_filter.categoryBits;
}

void FixtureComponent::set_filter(u16 category, u16 mask)
{
  b2Filter filter = my_fixture->GetFilterData();
  filter.categoryBits = category;
  filter.maskBits = mask;
  my_fixture->SetFilterData(filter);
}

f32 FixtureComponent::density() const
{
  return my_fixture->GetDensity();
}

void FixtureComponent::set_density(f32 density)
{
  my_fixture->SetDensity(density);
}

const RigidBodyComponent& FixtureComponent::rigid_body() const
{
  return my_body;
}

RigidBodyComponent& FixtureComponent::rigid_body()
{
  return my_body;
}

b2Fixture* FixtureComponent::fixture() const
{
  return my_fixture;
}

void FixtureComponent::create_fixture(const b2Shape &shape)
{
  if (my_fixture != nullptr) {
    my_filter = my_fixture->GetFilterData();
    destroy_fixture();
  }

  b2FixtureDef def;
  def.shape = &shape;
  def.userData = this;
  def.filter = my_filter;
  my_fixture = my_body.b2_body()->CreateFixture(&def);

}

void FixtureComponent::destroy_fixture()
{
  if (my_fixture != nullptr) {
    my_body.b2_body()->DestroyFixture(my_fixture);
    my_fixture = nullptr;
  }
}

void FixtureComponent::add_contact(FixtureComponent *component)
{
  my_contacts.push_back(component);
}

void FixtureComponent::clear_contacts()
{
  my_contacts.clear();
}

bool FixtureComponent::collide() const
{
  return !my_contacts.empty();
}

const std::vector<FixtureComponent *>& FixtureComponent::contacts() const
{
  return my_contacts;
}

b2Shape* FixtureComponent::b2_shape() const
{
  return my_fixture->GetShape();
}

BoxFixtureComponent::BoxFixtureComponent(const Vec2f &size, const Vec2f &center,
  RigidBodyComponent &body, Entity &entity)
  : FixtureComponent(body, entity)
{
  b2PolygonShape shape;
  shape.SetAsBox(size.x, size.y, b2Vec2(center.x, center.y), 0);
  create_fixture(shape);
}

void BoxFixtureComponent::set_size(f32 width, f32 height)
{
  b2PolygonShape shape;
  shape.SetAsBox(width, height);
  create_fixture(shape);
}

}
