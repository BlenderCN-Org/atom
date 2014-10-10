#pragma once

#include "component.h"

// Bullet forward declaration
class btRigidBody;
struct btDefaultMotionState;

namespace atom {

enum class RigidBodyType : u32 {
  STATIC,
  KINEMATIC,
  DYNAMIC
};

TYPE_OF(RigidBodyType, U32)
MAP_COMPONENT_TYPE(RigidBodyComponent, RIGID_BODY)

class RigidBodyComponent : public NullComponent {
  uptr<btRigidBody> my_rigid_body;
  uptr<btDefaultMotionState> my_motion_state;
  f32               my_mass;
  RigidBodyType     my_body_type;

  void activate() override;

  void deactivate() override;

  uptr<Component> clone() const override;
public:
  RigidBodyComponent();
  ~RigidBodyComponent();

  void set_body_type(RigidBodyType type)
  {
    my_body_type = type;
  }

  void set_mass(f32 mass)
  {
    my_mass = mass;
  }

  btRigidBody* get_rigid_body() const
  {
    return my_rigid_body.get();
  }
  
  META_SUB_CLASS(NullComponent);
};

}
