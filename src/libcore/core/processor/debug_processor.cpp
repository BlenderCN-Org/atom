#include "debug_processor.h"
#include <bullet/BulletDynamics/Dynamics/btDiscreteDynamicsWorld.h>
#include "../world/world.h"
#include "../video/uniforms.h"
#include "../system/resource_service.h"

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


DebugProcessor::DebugProcessor(VideoService &vs, ResourceService &rs, World &world)
  : my_vs(vs)
  , my_world(world)
  , my_debug_categories(0)
  , my_physics_drawer(new PhysicsDebugDrawer())
{
  my_wireframe_material = rs.get_material("lines");
  my_bounding_box_material = rs.get_material("debug_bounding_box");
  my_aabb_material = rs.get_material("debug_aabb");
}

DebugProcessor::~DebugProcessor()
{

}

void DebugProcessor::poll()
{
  my_physics_drawer->clear();
  my_world.processors().physics.bt_world().debugDrawWorld();
}

void DebugProcessor::start()
{
  my_world.processors().physics.bt_world().setDebugDrawer(my_physics_drawer.get());
}

void DebugProcessor::draw()
{
  draw_physics();
  draw_bounding_box();
  draw_aabb();
}

void DebugProcessor::draw_physics()
{
  Slice<Vec3f> lines = my_physics_drawer->get_lines();
  if (lines.size() == 0) {
    return;
  }

  Mesh mesh;

  uptr<VideoBuffer> vertex_buffer(new VideoBuffer(my_vs, VideoBufferUsage::STATIC_DRAW));
  vertex_buffer->set_bytes(lines.data(), lines.size() * sizeof(Vec3f));

  Uniforms &u = my_vs.get_uniforms();
  u.transformations.model = Mat4f();
  u.model = Mat4f();
  u.mvp = u.transformations.model_view_projection();
  RenderContext context = { u, my_vs };

  mesh.vertex = std::move(vertex_buffer);

  my_wireframe_material->material().draw_mesh(context, mesh);
}

void DebugProcessor::draw_bounding_box()
{
  auto entities = my_world.all_entities();
  std::vector<Vec3f> lines;
  lines.reserve(entities.size() * 24);


  for (const sptr<Entity> &entity : my_world.all_entities()) {
    Mat4f transform = entity->transform();
    const BoundingBox &box = entity->bounding_box();

    Vec3f v0 = transform * Vec3f(box.xmin, box.ymin, box.zmin);
    Vec3f v1 = transform * Vec3f(box.xmax, box.ymin, box.zmin);
    Vec3f v2 = transform * Vec3f(box.xmax, box.ymax, box.zmin);
    Vec3f v3 = transform * Vec3f(box.xmin, box.ymax, box.zmin);
    Vec3f v4 = transform * Vec3f(box.xmin, box.ymin, box.zmax);
    Vec3f v5 = transform * Vec3f(box.xmax, box.ymin, box.zmax);
    Vec3f v6 = transform * Vec3f(box.xmax, box.ymax, box.zmax);
    Vec3f v7 = transform * Vec3f(box.xmin, box.ymax, box.zmax);

    lines.push_back(v0);
    lines.push_back(v1);
    lines.push_back(v1);
    lines.push_back(v2);
    lines.push_back(v2);
    lines.push_back(v3);
    lines.push_back(v3);
    lines.push_back(v0);

    lines.push_back(v4);
    lines.push_back(v5);
    lines.push_back(v5);
    lines.push_back(v6);
    lines.push_back(v6);
    lines.push_back(v7);
    lines.push_back(v7);
    lines.push_back(v4);

    lines.push_back(v0);
    lines.push_back(v4);
    lines.push_back(v1);
    lines.push_back(v5);
    lines.push_back(v2);
    lines.push_back(v6);
    lines.push_back(v3);
    lines.push_back(v7);
  }

  Mesh mesh;
  uptr<VideoBuffer> vertex_buffer(new VideoBuffer(my_vs, VideoBufferUsage::STATIC_DRAW));
  vertex_buffer->set_bytes(lines.data(), lines.size() * sizeof(Vec3f));

  Uniforms &u = my_vs.get_uniforms();
  u.transformations.model = Mat4f();
  u.model = Mat4f();
  u.mvp = u.transformations.model_view_projection();
  RenderContext context = { u, my_vs };

  mesh.vertex = std::move(vertex_buffer);
  my_bounding_box_material->material().draw_mesh(context, mesh);
}

void DebugProcessor::draw_aabb()
{
  auto entities = my_world.all_entities();
  std::vector<Vec3f> lines;
  lines.reserve(entities.size() * 24);


  for (const sptr<Entity> &entity : my_world.all_entities()) {
    const BoundingBox &box = entity->aabb();

    Vec3f v0 = Vec3f(box.xmin, box.ymin, box.zmin);
    Vec3f v1 = Vec3f(box.xmax, box.ymin, box.zmin);
    Vec3f v2 = Vec3f(box.xmax, box.ymax, box.zmin);
    Vec3f v3 = Vec3f(box.xmin, box.ymax, box.zmin);
    Vec3f v4 = Vec3f(box.xmin, box.ymin, box.zmax);
    Vec3f v5 = Vec3f(box.xmax, box.ymin, box.zmax);
    Vec3f v6 = Vec3f(box.xmax, box.ymax, box.zmax);
    Vec3f v7 = Vec3f(box.xmin, box.ymax, box.zmax);

    lines.push_back(v0);
    lines.push_back(v1);
    lines.push_back(v1);
    lines.push_back(v2);
    lines.push_back(v2);
    lines.push_back(v3);
    lines.push_back(v3);
    lines.push_back(v0);

    lines.push_back(v4);
    lines.push_back(v5);
    lines.push_back(v5);
    lines.push_back(v6);
    lines.push_back(v6);
    lines.push_back(v7);
    lines.push_back(v7);
    lines.push_back(v4);

    lines.push_back(v0);
    lines.push_back(v4);
    lines.push_back(v1);
    lines.push_back(v5);
    lines.push_back(v2);
    lines.push_back(v6);
    lines.push_back(v3);
    lines.push_back(v7);
  }

  Mesh mesh;
  uptr<VideoBuffer> vertex_buffer(new VideoBuffer(my_vs, VideoBufferUsage::STATIC_DRAW));
  vertex_buffer->set_bytes(lines.data(), lines.size() * sizeof(Vec3f));

  Uniforms &u = my_vs.get_uniforms();
  u.transformations.model = Mat4f();
  u.model = Mat4f();
  u.mvp = u.transformations.model_view_projection();
  RenderContext context = { u, my_vs };

  mesh.vertex = std::move(vertex_buffer);
  my_aabb_material->material().draw_mesh(context, mesh);
}

}
