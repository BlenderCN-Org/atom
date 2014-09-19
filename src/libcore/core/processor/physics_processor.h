#pragma once

#include <vector>
#include "../foundation.h"
#include "processor.h"

namespace atom {

class BoxColliderComponent;
class SphereColliderComponent;
class MeshColliderComponent;


//
// PhysicsProcessor
//

class PhysicsProcessor : public Processor {
  VideoService                         &my_vs;
  ResourceService                      &my_rs;
  bool                                  my_is_running;
  uptr<btDefaultCollisionConfiguration> my_configuration;
  uptr<btCollisionDispatcher>           my_dispatcher;
  uptr<btDbvtBroadphase>                my_broadphase;
  uptr<btSequentialImpulseConstraintSolver> my_solver;
  uptr<btDiscreteDynamicsWorld>         my_world;
  std::vector<RigidBodyComponent *>     my_bodies;

public:
  PhysicsProcessor(VideoService &vs, ResourceService &rs);
  ~PhysicsProcessor();

  uptr<BoxColliderComponent> create_box_collider();

  void poll() override;

  void start() override;

  void register_rigid_body(RigidBodyComponent *rigid_body);
  void unregister_rigid_body(RigidBodyComponent *rigid_body);

  btDiscreteDynamicsWorld& bt_world() const;
};

}
