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
// PhysicsDebugDrawer
//

template<typename T>
Slice<T> to_slice(const std::vector<T> &array)
{
  return Slice<T>(array.data(), array.size());
}

class PhysicsDebugDrawer : public btIDebugDraw {
  std::vector<Vec3f> my_vertices;

public:
  void clear()
  {
    my_vertices.clear();
  }

  Slice<Vec3f> get_lines() const
  {
    return to_slice(my_vertices);
  }

  void drawLine(const btVector3 &from, const btVector3 &to,
    const btVector3 &color) override
  {
    my_vertices.push_back(to_vec3(from));
    my_vertices.push_back(to_vec3(to));
  }

  void drawContactPoint(const btVector3& PointOnB,const btVector3& normalOnB,
    btScalar distance,int lifeTime,const btVector3& color) override
  {

  }

  void reportErrorWarning(const char* text) override
  {
    log::warning("%s", text);
  }

  void draw3dText(const btVector3& location,const char* textString) override
  {

  }

  void setDebugMode(int debugMode) override
  {

  }

  int getDebugMode() const override
  {
    return DBG_DrawWireframe;
  }
};


//
// PhysicsProcessor
//

PhysicsProcessor::PhysicsProcessor(VideoService &vs, ResourceService &rs)
  : my_vs(vs)
  , my_rs(rs)
  , my_is_running(false)
  , my_configuration(new btDefaultCollisionConfiguration())
  , my_dispatcher(new btCollisionDispatcher(my_configuration.get()))
  , my_broadphase(new btDbvtBroadphase())
  , my_solver(new btSequentialImpulseConstraintSolver())
  , my_world(new btDiscreteDynamicsWorld(my_dispatcher.get(),
      my_broadphase.get(), my_solver.get(), my_configuration.get()))
  , my_debug_drawer(new PhysicsDebugDrawer())
{
  my_world->setGravity(btVector3(0, 0, -ACCELERATION));
  my_world->setDebugDrawer(my_debug_drawer.get());
  //  set_enable_debug(true);
}

PhysicsProcessor::~PhysicsProcessor()
{

}

void PhysicsProcessor::poll()
{
  if (!my_is_running) {
    return;
  }

  my_debug_drawer->clear();

  // perform simulation step
  my_world->stepSimulation(1.0f / FPS, 10);
  my_world->debugDrawWorld();
}

void PhysicsProcessor::start()
{
  assert(my_is_running == false);
  my_is_running = true;
}

void PhysicsProcessor::debug_render()
{
  Slice<Vec3f> lines = my_debug_drawer->get_lines();
  if (lines.size() == 0) {
    return;
  }

  MaterialResourcePtr material = my_rs.get_material("lines");

  Mesh mesh;

  uptr<VideoBuffer> vertex_buffer(new VideoBuffer(my_vs));
  vertex_buffer->set_bytes(lines.data(), lines.size() * sizeof(Vec3f));

  Uniforms &u = my_vs.get_uniforms();
  u.transformations.model = Mat4f();
  u.model = Mat4f();
  u.mvp = u.transformations.model_view_projection();
  RenderContext context = { u, my_vs };

  mesh.vertex = std::move(vertex_buffer);

  material->material().draw_mesh(context, mesh);
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

}
