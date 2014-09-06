#pragma once

#include <vector>
#include "../foundation.h"
#include "processor.h"

// Bullet forward declarations
class btDefaultCollisionConfiguration;
class btCollisionDispatcher;
struct btDbvtBroadphase;
class btSequentialImpulseConstraintSolver;
class btDiscreteDynamicsWorld;

namespace atom {

class BoxColliderComponent;
class SphereColliderComponent;
class MeshColliderComponent;

class PhysicsDebugDrawer;

class PhysicsProcessor : public Processor {
  VideoService                         &my_vs;
  ResourceService                      &my_rs;
  bool                                  my_is_running;
  uptr<btDefaultCollisionConfiguration> my_configuration;
  uptr<btCollisionDispatcher>           my_dispatcher;
  uptr<btDbvtBroadphase>                my_broadphase;
  uptr<btSequentialImpulseConstraintSolver> my_solver;
  uptr<btDiscreteDynamicsWorld>         my_world;
  uptr<PhysicsDebugDrawer>              my_debug_drawer;
  std::vector<RigidBodyComponent *>     my_bodies;

public:
  PhysicsProcessor(VideoService &vs, ResourceService &rs);
  ~PhysicsProcessor();

  uptr<BoxColliderComponent> create_box_collider();

  void poll() override;

  void start() override;

  void debug_render();

  void register_rigid_body(RigidBodyComponent *rigid_body);
  void unregister_rigid_body(RigidBodyComponent *rigid_body);
};

}
