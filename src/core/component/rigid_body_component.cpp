#include "rigid_body_component.h"
#include "core/world/world.h"
#include "core/processor/physics_processor.h"
#include "core/utils/b2_utils.h"

namespace atom {

RigidBodyComponent::RigidBodyComponent(RigidBodyType type, Entity &entity)
  : Component(entity)
  , my_type(type)
  , my_update_entity_position(true)
  , my_fixed_rotation(false)
  , my_is_platform(false)
  , my_body(nullptr)
{
  b2BodyDef def;
  def.position.x = entity.position().x;
  def.position.y = entity.position().y;
//  def.gravityScale = component->gravity_scale();
  def.userData = this;

  if (type == RigidBodyType::DYNAMIC) {
    def.type = b2_dynamicBody;
  } else if (type == RigidBodyType::STATIC) {
    def.type = b2_staticBody;
  } else if (type == RigidBodyType::KINEMATIC) {
    def.type = b2_kinematicBody;
  } else {
    log::warning("Invalid body type");
  }

  my_body = processors().physics.b2_world().CreateBody(&def);
  my_body->SetFixedRotation(my_fixed_rotation);
}

f32 RigidBodyComponent::mass() const
{
  return my_body->GetMass();
}

void RigidBodyComponent::on_attach()
{
  world().processors().physics.add_rigid_body(this);
}

void RigidBodyComponent::on_detach()
{
  world().processors().physics.remove_rigid_body(this);
}

Vec2f RigidBodyComponent::position() const
{
  return to_vec2f(my_body->GetPosition());
}

RigidBodyType RigidBodyComponent::type() const
{
  return my_type;
}

bool RigidBodyComponent::is_entity_update_enabled() const
{
  return my_update_entity_position;
}

void RigidBodyComponent::set_update_entity_position(bool update)
{
  my_update_entity_position = update;
}

Vec2f RigidBodyComponent::linear_velocity() const
{
  return to_vec2f(my_body->GetLinearVelocity());
}

void RigidBodyComponent::set_linear_velocity(const Vec2f &v)
{
  my_body->SetLinearVelocity(to_b2_vec2(v));
}

void RigidBodyComponent::set_angle(f32 angle)
{
  b2Transform transform = my_body->GetTransform();
  my_body->SetTransform(transform.p, angle);
}

void RigidBodyComponent::set_position(const Vec2f &p)
{
  b2Transform transform = my_body->GetTransform();
  my_body->SetTransform(b2Vec2(p.x, p.y), transform.q.GetAngle());
}

void RigidBodyComponent::apply_force(const Vec2f &force)
{
  assert(my_body != nullptr);
  my_body->ApplyForceToCenter(to_b2_vec2(force), true);
}

void RigidBodyComponent::apply_impulse(const Vec2f &impulse)
{
  my_body->ApplyLinearImpulse(to_b2_vec2(impulse), my_body->GetWorldCenter(), true);
}

bool RigidBodyComponent::fixed_rotation() const
{
  return my_fixed_rotation;
}

void RigidBodyComponent::set_fixed_rotation(bool enable)
{
  my_fixed_rotation = enable;
}

bool RigidBodyComponent::is_platform() const
{
  return my_is_platform;
}

void RigidBodyComponent::set_platform(bool platform)
{
  my_is_platform = true;
}

b2Body* RigidBodyComponent::b2_body() const
{
  return my_body;
}

}
