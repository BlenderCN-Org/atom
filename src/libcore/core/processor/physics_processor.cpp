#include "physics_processor.h"
#include <bullet/btBulletDynamicsCommon.h>
#include "../system/resource_service.h"
#include "../video/video_service.h"
#include "../video/mesh.h"
#include "../video/uniforms.h"
#include "../component/rigid_body_component.h"
#include "../utils/bt_utils.h"
#include "../constants.h"

namespace atom {



//
// PhysicsProcessor
//

PhysicsProcessor::PhysicsProcessor(World &world)
  : NullProcessor(world)
  , my_is_running(false)
  , my_configuration(new btDefaultCollisionConfiguration())
  , my_dispatcher(new btCollisionDispatcher(my_configuration.get()))
  , my_broadphase(new btDbvtBroadphase())
  , my_solver(new btSequentialImpulseConstraintSolver())
  , my_world(new btDiscreteDynamicsWorld(my_dispatcher.get(),
      my_broadphase.get(), my_solver.get(), my_configuration.get()))
{
  my_world->setGravity(btVector3(0, 0, -ACCELERATION));

  //  set_enable_debug(true);
}

PhysicsProcessor::~PhysicsProcessor()
{

}

void PhysicsProcessor::activate()
{
  assert(my_is_running == false);
  my_is_running = true;
}

void PhysicsProcessor::poll()
{
  if (!my_is_running) {
    return;
  }

  // perform simulation step
  my_world->stepSimulation(1.0f / FPS, 10);
}

void PhysicsProcessor::register_rigid_body(RigidBodyComponent *rigid_body)
{
  assert(rigid_body != nullptr);
  my_bodies.push_back(rigid_body);

  btRigidBody *bt_rigid_body = rigid_body->get_rigid_body();

  my_world->addRigidBody(bt_rigid_body);
}

void PhysicsProcessor::unregister_rigid_body(RigidBodyComponent *rigid_body)
{
  my_bodies.erase(std::remove(my_bodies.begin(), my_bodies.end(),
     rigid_body), my_bodies.end());
  my_world->removeRigidBody(rigid_body->get_rigid_body());
}

btDiscreteDynamicsWorld& PhysicsProcessor::bt_world() const
{
  return *my_world;
}

}
