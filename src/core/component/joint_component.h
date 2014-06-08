#pragma once

#include <Box2D/Box2D.h>
#include "core/component/component.h"
#include "core/component/rigid_body_component.h"
#include "core/component/fixture_component.h"

namespace atom {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

class JointComponent : public Component {
public:
  JointComponent(Entity &entity);

  void on_attach() override;

  void on_detach() override;

  b2Joint* b2_joint() const;

  void create_joint(const b2JointDef &def);
  void destroy_joint();

private:
  b2Joint          *my_joint;
};

class RopeJointComponent : public JointComponent {
public:
  RopeJointComponent(RigidBodyComponent &body, Entity &entity);

  void tie(const Vec2f &hook_position);

  void cut();

  bool is_tied() const;

  f32 length() const;

  void set_length(f32 length);

private:
  void create();

  b2RopeJoint* b2_rope_joint() const;

private:
  f32                 my_length;
  b2RopeJointDef      my_def;
  RigidBodyComponent &my_object;
  RigidBodyComponent  my_hook;
  BoxFixtureComponent my_hook_fixture;
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

class FixedJointComponent : public JointComponent {
public:
  FixedJointComponent(RigidBodyComponent &body, Entity &entity);

  void fix(const Vec2f &position);

  void cut();

private:
  void create();

private:
  RigidBodyComponent &my_object;
  b2WeldJointDef      my_def;
  RigidBodyComponent  my_hook;
  BoxFixtureComponent my_fixture;

};

}
