#include "joint_component.h"
#include "core/processor/physics_processor.h"
#include "core/world/world.h"

namespace atom {

JointComponent::JointComponent(Entity &entity)
  : Component(entity)
  , my_joint(nullptr)
{
}

void JointComponent::on_attach()
{
  processors().physics.add_joint(this);
}

void JointComponent::on_detach()
{
  processors().physics.remove_joint(this);
}

b2Joint *JointComponent::b2_joint() const
{
  return my_joint;
}

void JointComponent::create_joint(const b2JointDef &def)
{
  destroy_joint();
  my_joint = processors().physics.b2_world().CreateJoint(&def);
}

void JointComponent::destroy_joint()
{
  if (my_joint != nullptr) {
    processors().physics.b2_world().DestroyJoint(my_joint);
    my_joint = nullptr;
  }
}


RopeJointComponent::RopeJointComponent(RigidBodyComponent &body, Entity &entity)
  : JointComponent(entity)
  , my_length(10)
  , my_object(body)
  , my_hook(RigidBodyType::STATIC, entity)
  , my_hook_fixture(Vec2f(0.1, 0.1), Vec2f(0, 0), my_hook, entity)
{
  my_hook.set_update_entity_position(false);
//  create();
}

void RopeJointComponent::tie(const Vec2f &hook_position)
{
  my_hook.set_position(hook_position);
  create();
}

void RopeJointComponent::cut()
{
  destroy_joint();
}

bool RopeJointComponent::is_tied() const
{
  return b2_joint() != nullptr;
}

f32 RopeJointComponent::length() const
{
  return my_length;
}

void RopeJointComponent::set_length(f32 length)
{
  my_length = std::max(length, 0.0f);

  b2RopeJoint *joint = b2_rope_joint();

  if (joint != nullptr) {
    joint->SetMaxLength(my_length);
  }
}

void RopeJointComponent::create()
{
  b2RopeJointDef def;
  def.bodyA = my_object.b2_body();
  def.bodyB = my_hook.b2_body();
  def.localAnchorA = b2Vec2(0, 0);
  def.localAnchorB = b2Vec2(0, 0);
  def.maxLength = my_length;
  create_joint(def);
}

b2RopeJoint *RopeJointComponent::b2_rope_joint() const
{
  return safe_cast<b2RopeJoint *>(b2_joint());
}

FixedJointComponent::FixedJointComponent(RigidBodyComponent &body, Entity &entity)
  : JointComponent(entity)
  , my_object(body)
  , my_hook(RigidBodyType::STATIC, entity)
  , my_fixture(Vec2f(0.1, 0.1), Vec2f(0, 0), my_hook, entity)
{
  create();
}

void FixedJointComponent::fix(const Vec2f &position)
{
  my_hook.set_position(position);
  create();
}

void FixedJointComponent::cut()
{
  destroy_joint();
}

void FixedJointComponent::create()
{
  b2DistanceJointDef def;
  def.bodyA = my_object.b2_body();
  def.bodyB = my_hook.b2_body();
  def.collideConnected = false;
  def.localAnchorA = b2Vec2(0, 0);
  def.localAnchorB = b2Vec2(0, 0);
  create_joint(def);
}

}
