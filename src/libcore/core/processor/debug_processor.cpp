#include "debug_processor.h"
#include <bullet/BulletDynamics/Dynamics/btDiscreteDynamicsWorld.h>
#include <bullet/btBulletDynamicsCommon.h>
#include "../utils/bt_utils.h"
#include "../world/world.h"
#include "../video/uniforms.h"
#include "../system/resource_service.h"
#include "../processor/physics_processor.h"
#include "../component/geometry_component.h"
#include "../video/render_context.h"

namespace atom {

//
// PhysicsDebugDrawer
//


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


DebugProcessor::DebugProcessor(World &world)
  : NullProcessor(world)
  , my_debug_categories(0)
  , my_physics_drawer(new PhysicsDebugDrawer())
{
  ResourceService &rs = core().resource_service();
  my_wireframe_material = rs.get_material("debug_wireframe");
  my_lines_material = rs.get_material("lines");
  my_bounding_box_material = rs.get_material("debug_bounding_box");
  my_aabb_material = rs.get_material("debug_aabb");
}

DebugProcessor::~DebugProcessor()
{

}

void DebugProcessor::activate()
{
  world().processors().physics.bt_world().setDebugDrawer(my_physics_drawer.get());
}

void DebugProcessor::poll()
{
  if (my_debug_categories & DebugCategory::PHYSICS) {
    my_physics_drawer->clear();
    world().processors().physics.bt_world().debugDrawWorld();
  }
}

void DebugProcessor::draw()
{
  if (my_debug_categories & DebugCategory::PHYSICS) {
    draw_physics();
  }

  if (my_debug_categories & DebugCategory::BOUNDING_BOX) {
    draw_bounding_box();
  }

  if (my_debug_categories & DebugCategory::AABB) {
    draw_aabb();
  }
  
  if (my_debug_categories & DebugCategory::GEOMETRY_CACHE) {
    draw_geometry_cache();
  }
}

void DebugProcessor::set_debug(u32 category, bool enable)
{
  if (enable) {
    my_debug_categories |= category;
  } else {
    my_debug_categories &= ~category;
  }
}

void DebugProcessor::draw_wireframe(const Slice<Vec3f> &vertices,
  const Slice<u32> &indices, const Vec3f &color, const Mat4f &model)
{
  Mesh mesh;
  VideoService &vs = core().video_service();
  uptr<VideoBuffer> vertex_buffer(new VideoBuffer(vs, VideoBufferUsage::STATIC_DRAW));
  uptr<VideoBuffer> index_buffer(new VideoBuffer(vs, VideoBufferUsage::STATIC_DRAW));
  vertex_buffer->set_bytes(vertices.data(), vertices.raw_size());
  index_buffer->set_bytes(indices.data(), indices.raw_size());
  
  Uniforms &u = vs.get_uniforms();
  u.transformations.model = model;
  u.model = model;
  u.mvp = u.transformations.model_view_projection();
  u.color = color;  // TODO: farba sa nastavuje priamo v materiali a nie tu
  
  RenderContext context = { u, vs };
  mesh.vertex = std::move(vertex_buffer);
  mesh.surface = std::move(index_buffer);
  
  Material &material = my_wireframe_material->material();
  vs.set_draw_face(material.face());
  material.draw_mesh(context, mesh);
}

void DebugProcessor::draw_physics()
{
  Slice<Vec3f> lines = my_physics_drawer->get_lines();
  if (lines.size() == 0) {
    return;
  }

  Mesh mesh;

  VideoService &vs = core().video_service();

  uptr<VideoBuffer> vertex_buffer(new VideoBuffer(vs, VideoBufferUsage::STATIC_DRAW));
  vertex_buffer->set_bytes(lines.data(), lines.size() * sizeof(Vec3f));

  Uniforms &u = vs.get_uniforms();
  u.transformations.model = Mat4f();
  u.model = Mat4f();
  u.mvp = u.transformations.model_view_projection();
  RenderContext context = { u, vs };

  mesh.vertex = std::move(vertex_buffer);

  my_lines_material->material().draw_mesh(context, mesh);
}

void DebugProcessor::draw_bounding_box()
{
  auto entities = world().all_entities();
  std::vector<Vec3f> lines;
  lines.reserve(entities.size() * 24);


  for (const sptr<Entity> &entity : world().all_entities()) {
    Mat4f transform = entity->transform();
    const BoundingBox &box = entity->bounding_box();

    Vec3f v0 = transform_point(transform, Vec3f(box.xmin, box.ymin, box.zmin));
    Vec3f v1 = transform_point(transform, Vec3f(box.xmax, box.ymin, box.zmin));
    Vec3f v2 = transform_point(transform, Vec3f(box.xmax, box.ymax, box.zmin));
    Vec3f v3 = transform_point(transform, Vec3f(box.xmin, box.ymax, box.zmin));
    Vec3f v4 = transform_point(transform, Vec3f(box.xmin, box.ymin, box.zmax));
    Vec3f v5 = transform_point(transform, Vec3f(box.xmax, box.ymin, box.zmax));
    Vec3f v6 = transform_point(transform, Vec3f(box.xmax, box.ymax, box.zmax));
    Vec3f v7 = transform_point(transform, Vec3f(box.xmin, box.ymax, box.zmax));

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
  VideoService &vs = core().video_service();
  uptr<VideoBuffer> vertex_buffer(new VideoBuffer(vs, VideoBufferUsage::STATIC_DRAW));
  vertex_buffer->set_bytes(lines.data(), lines.size() * sizeof(Vec3f));

  Uniforms &u = vs.get_uniforms();
  u.transformations.model = Mat4f();
  u.model = Mat4f();
  u.mvp = u.transformations.model_view_projection();
  RenderContext context = { u, vs };

  mesh.vertex = std::move(vertex_buffer);
  my_bounding_box_material->material().draw_mesh(context, mesh);
}

void DebugProcessor::draw_aabb()
{
  auto entities = world().all_entities();
  std::vector<Vec3f> lines;
  lines.reserve(entities.size() * 24);


  for (const sptr<Entity> &entity : world().all_entities()) {
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
  VideoService &vs = core().video_service();
  uptr<VideoBuffer> vertex_buffer(new VideoBuffer(vs, VideoBufferUsage::STATIC_DRAW));
  vertex_buffer->set_bytes(lines.data(), lines.size() * sizeof(Vec3f));

  Uniforms &u = vs.get_uniforms();
  u.transformations.model = Mat4f();
  u.model = Mat4f();
  u.mvp = u.transformations.model_view_projection();
  RenderContext context = { u, vs };

  mesh.vertex = std::move(vertex_buffer);
  my_aabb_material->material().draw_mesh(context, mesh);
}

void DebugProcessor::draw_geometry_cache()
{
  core().video_service().disable_depth_test();
  
  for (const sptr<Entity> &entity : world().all_entities()) {
    std::vector<GeometryComponent *> components =
      entity->find_components<GeometryComponent>();
    
    for (GeometryComponent *component : components) {
      const GeometryCache &geometry_cache = component->geometry_cache();
      
      if (geometry_cache.vertices.empty()) {
        continue;
      }
      
      const Model *model = component->model();
      
      Slice<u32> indices = model->find_stream<u32>("indices");
      
      const Slice<Vec3f> vertices = to_slice(geometry_cache.vertices);
      
      draw_wireframe(vertices, indices, Vec3f(1, 0.5, 1), entity->transform());
    }
  }
  
  core().video_service().enable_depth_test();
}

}
