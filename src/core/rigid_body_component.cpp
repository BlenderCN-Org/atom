#include "rigid_body_component.h"
#include "collider_component.h"
#include <bullet/btBulletDynamicsCommon.h>
#include "bt_utils.h"
#include "world.h"
#include "physics_processor.h"

namespace atom {

META_CLASS(RigidBodyComponent,
  FIELD(my_body_type, "body_type"),
  FIELD(my_mass, "mass")
)

void RigidBodyComponent::activate()
{
  ColliderComponent *collider = entity().find_component<ColliderComponent>();

  if (collider == nullptr) {
    log_error("Entity doesn't contain collider component");
    return;
  }

  btCollisionShape *shape = collider->get_collision_shape();

  if (shape == nullptr) {
    log_error("ColliderComponent doesn't contains collision shape");
    return;
  }

  btTransform transform;
  const Mat4f &entity_transform = entity().transform();
  transform.setFromOpenGLMatrix(&entity_transform[0][0]);
  my_motion_state.reset(new btDefaultMotionState(transform));
  my_rigid_body.reset(new btRigidBody(my_mass, my_motion_state.get(), shape));

  switch (my_body_type) {
    case RigidBodyType::STATIC:
      log_info("Creating static body");
      my_rigid_body->setFlags(btCollisionObject::CF_STATIC_OBJECT);
      break;

    case RigidBodyType::KINEMATIC:
      my_rigid_body->setFlags(btCollisionObject::CF_KINEMATIC_OBJECT);
      break;

    case RigidBodyType::DYNAMIC:
      // rigid body is dynamic by default
      my_rigid_body->setFlags(0);
      break;
  }

  PhysicsProcessor &pp = world().processors().physics;
  pp.register_rigid_body(this);
//  btMotionState *motion_state = my_rigid_body->getMotionState();
//  motion_state->setWorldTransform(transform);
}

void RigidBodyComponent::deactivate()
{
  world().processors().physics.unregister_rigid_body(this);
  my_rigid_body.reset();
  my_motion_state.reset();
}

RigidBodyComponent::RigidBodyComponent()
  : NullComponent(ComponentType::RIGID_BODY)
  , my_mass(1.0f)
  , my_body_type(RigidBodyType::DYNAMIC)
{
  META_INIT();
}

RigidBodyComponent::~RigidBodyComponent()
{
  // empty
}

}
