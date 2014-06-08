#pragma once

#include <Box2D/Box2D.h>
#include <vector>
#include "core/corefwd.h"
#include "core/math/mat.h"
#include "processor.h"

namespace atom {

class Box2DDebugDraw;

class PhysicsProcessor : public Processor, b2ContactListener {
public:
  PhysicsProcessor(VideoService &vs, ResourceService &rs);
  ~PhysicsProcessor();

  void add_fixture(FixtureComponent *component);
  void remove_fixture(FixtureComponent *component);

  void add_rigid_body(RigidBodyComponent *component);
  void remove_rigid_body(RigidBodyComponent *component);

  void add_joint(JointComponent *component);
  void remove_joint(JointComponent *component);

  void poll() override;

  void start() override;

  void ray_cast(const Vec2f &origin, const Vec2f &dir, b2RayCastCallback &callback) const;

  bool is_debug_enabled() const;

  void set_enable_debug(bool enable);

  void debug_draw(const Camera &camera);

  b2World& b2_world();

  void PreSolve(b2Contact *contact, const b2Manifold *oldManifold) override;

  void PostSolve(b2Contact *contact, const b2ContactImpulse *impulse) override;

  void BeginContact(b2Contact *contact) override;

  void EndContact(b2Contact *contact) override;

private:
  void process_fixtures();

  void disable_all_ghost_contacts();

  void add_ghost_contact(b2Contact *contact);

  void remove_ghost_contact(b2Contact *contact);

private:
  bool                              my_is_running;
  b2World                           my_world;
  bool                              my_is_debug_enabled;
  uptr<Box2DDebugDraw>              my_debug_draw;
  std::vector<FixtureComponent *>   my_fixtures;
  std::vector<RigidBodyComponent *> my_components;
  std::vector<JointComponent *>     my_joints;
  std::vector<b2Contact *>          my_ghost_contacts;
};

}
