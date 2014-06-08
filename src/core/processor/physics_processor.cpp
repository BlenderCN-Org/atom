#include "core/processor/physics_processor.h"
#include "core/processor/box2d_debug_draw.h"
#include "core/component/rigid_body_component.h"
#include "core/component/fixture_component.h"
#include "core/component/joint_component.h"
#include "core/utils/utils.h"
#include "core/utils/b2_utils.h"

namespace atom {

PhysicsProcessor::PhysicsProcessor(VideoService &vs, ResourceService &rs)
  : my_is_running(false)
  , my_world(b2Vec2(0, 0))
  , my_is_debug_enabled(false)
  , my_debug_draw(new Box2DDebugDraw(vs, rs))
{
//  set_enable_debug(true);
  my_world.SetContactListener(this);
}

PhysicsProcessor::~PhysicsProcessor()
{

}

void PhysicsProcessor::poll()
{
  if (!my_is_running) {
    return;
  }

  if (my_is_debug_enabled) {
    my_debug_draw->reset();
  }

  my_world.Step(stf(1), 6, 2);

  process_fixtures();
}

void PhysicsProcessor::start()
{
  assert(my_is_running == false);
  my_is_running = true;
  my_world.SetGravity(b2Vec2(0, -19.8));
}

void PhysicsProcessor::ray_cast(const Vec2f &origin, const Vec2f &dir, b2RayCastCallback &callback) const
{
  my_world.RayCast(&callback, to_b2_vec2(origin), to_b2_vec2(origin + dir));
}

bool PhysicsProcessor::is_debug_enabled() const
{
  return my_is_debug_enabled;
}

void PhysicsProcessor::set_enable_debug(bool enable)
{
  my_world.SetDebugDraw(enable ? my_debug_draw.get() : nullptr);
  my_is_debug_enabled = enable;
}

void PhysicsProcessor::debug_draw(const Camera &camera)
{
  if (!my_is_running) {
    return;
  }

  my_world.DrawDebugData();
  my_debug_draw->draw(camera);
}

b2World& PhysicsProcessor::b2_world()
{
  return my_world;
}

void PhysicsProcessor::PreSolve(b2Contact *contact, const b2Manifold *oldManifold)
{
  if (std::find(my_ghost_contacts.begin(), my_ghost_contacts.end(), contact) != my_ghost_contacts.end()) {
    contact->SetEnabled(false);
//    log::info("Disabling the contact");
  }
//  log::info("PreSolve");
}

void PhysicsProcessor::PostSolve(b2Contact *contact, const b2ContactImpulse *impulse)
{
//  log::info("PostSolve");
}

void PhysicsProcessor::BeginContact(b2Contact *contact)
{
//  log::info("BeginContact");

  b2Body *b2_body_a = contact->GetFixtureA()->GetBody();
  b2Body *b2_body_b = contact->GetFixtureB()->GetBody();

  assert(b2_body_a != nullptr);
  assert(b2_body_b != nullptr);

  RigidBodyComponent *component_a = reinterpret_cast<RigidBodyComponent *>(b2_body_a->GetUserData());
  RigidBodyComponent *component_b = reinterpret_cast<RigidBodyComponent *>(b2_body_b->GetUserData());

  assert(component_a != nullptr);
  assert(component_b != nullptr);

  RigidBodyComponent *other = nullptr;

  if (component_a->is_platform() && !component_b->is_platform()) {
    other = component_b;
  } else if (!component_a->is_platform() && component_b->is_platform()) {
    other = component_a;
  } else {
    return;
  }

  if (other->linear_velocity().y > 0) {
    contact->SetEnabled(false);
    add_ghost_contact(contact);
  }
}

void PhysicsProcessor::EndContact(b2Contact *contact)
{
//  log::info("EndContact");
  remove_ghost_contact(contact);
  contact->SetEnabled(true);
}

void PhysicsProcessor::process_fixtures()
{
  for (FixtureComponent *component : my_fixtures) {
    component->clear_contacts();
  }

  // process fixtures
  b2Contact *contact = my_world.GetContactList();

  while (contact != nullptr) {
    // only touching contacts are added to the component contact list
    if (contact->IsTouching()) {
      FixtureComponent *a = reinterpret_cast<FixtureComponent *>(contact->GetFixtureA()->GetUserData());
      FixtureComponent *b = reinterpret_cast<FixtureComponent *>(contact->GetFixtureB()->GetUserData());

      a->add_contact(b);
      b->add_contact(a);
    }

    contact = contact->GetNext();
  }
}

void PhysicsProcessor::add_ghost_contact(b2Contact *contact)
{
  my_ghost_contacts.push_back(contact);
}

void PhysicsProcessor::remove_ghost_contact(b2Contact *contact)
{
  my_ghost_contacts.erase(std::remove(my_ghost_contacts.begin(), my_ghost_contacts.end(), contact), my_ghost_contacts.end());
}

void PhysicsProcessor::add_fixture(FixtureComponent *component)
{
  assert(component != nullptr);
  my_fixtures.push_back(component);
}

void PhysicsProcessor::remove_fixture(FixtureComponent *component)
{
  assert(component != nullptr);
  // QQQ delete fixture, fixture is deleted when body is deleted
  my_fixtures.erase(std::remove(my_fixtures.begin(), my_fixtures.end(), component), my_fixtures.end());
}

void PhysicsProcessor::add_rigid_body(RigidBodyComponent *component)
{
  assert(component != nullptr);
  my_components.push_back(component);
}

void PhysicsProcessor::remove_rigid_body(RigidBodyComponent *component)
{
  assert(component != nullptr);
  my_components.erase(std::remove(my_components.begin(), my_components.end(), component), my_components.end());
}

void PhysicsProcessor::add_joint(JointComponent *component)
{
  assert(component != nullptr);
  my_joints.push_back(component);
}

void PhysicsProcessor::remove_joint(JointComponent *component)
{
  assert(component != nullptr);
  my_joints.erase(std::remove(my_joints.begin(), my_joints.end(), component), my_joints.end());
}

}
