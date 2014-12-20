#include "debug_processor.h"
#include <bullet/BulletDynamics/Dynamics/btDiscreteDynamicsWorld.h>
#include <bullet/btBulletDynamicsCommon.h>
#include "bt_utils.h"
#include "world.h"
#include "uniforms.h"
#include "resource_service.h"
#include "physics_processor.h"
#include "geometry_component.h"
#include "render_context.h"
#include "model.h"

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
  my_debug_material = rs.get_material("debug");
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
    gather_physics();
  }

  if (my_debug_categories & DebugCategory::BOUNDING_BOX) {
    gather_bounding_box();
  }

  if (my_debug_categories & DebugCategory::AABB) {
    gather_aabb();
  }

  if (my_debug_categories & DebugCategory::GEOMETRY_CACHE) {
    gather_geometry_cache();
  }

  draw_all();
  clear();
}

void DebugProcessor::set_debug(u32 category, bool enable)
{
  if (enable) {
    my_debug_categories |= category;
  } else {
    my_debug_categories &= ~category;
  }
}

void DebugProcessor::draw_line(const Vec3f &start, const Vec3f &end, const Vec3f &color)
{
  my_line_points.push_back(start);
  my_line_points.push_back(end);
  my_line_colors.push_back(color);
  my_line_colors.push_back(color);
}

void DebugProcessor::draw_line(const Mat4f &transform, const Vec3f &start, const Vec3f &end, const Vec3f &color)
{
  draw_line(transform_point(transform, start), transform_point(transform, end), color);
}

void DebugProcessor::clear()
{
  // TODO: maybe use swap
  my_line_points.clear();
  my_line_colors.clear();
}

void DebugProcessor::gather_physics()
{
  not_implemented();
}

void DebugProcessor::gather_bounding_box()
{
  auto entities = world().all_entities();
  std::vector<Vec3f> lines;
  lines.reserve(entities.size() * 24);
  const Vec3f color(rgb_to_vec3f(0xca5779));

  for (const sptr<Entity> &entity : world().all_entities()) {
    Mat4f transform = entity->transform();
    const BoundingBox &box = entity->bounding_box();

    const Vec3f v[] = {
      transform_point(transform, Vec3f(box.xmin, box.ymin, box.zmin)),
      transform_point(transform, Vec3f(box.xmax, box.ymin, box.zmin)),
      transform_point(transform, Vec3f(box.xmax, box.ymax, box.zmin)),
      transform_point(transform, Vec3f(box.xmin, box.ymax, box.zmin)),
      transform_point(transform, Vec3f(box.xmin, box.ymin, box.zmax)),
      transform_point(transform, Vec3f(box.xmax, box.ymin, box.zmax)),
      transform_point(transform, Vec3f(box.xmax, box.ymax, box.zmax)),
      transform_point(transform, Vec3f(box.xmin, box.ymax, box.zmax))
    };

    draw_line(v[0], v[1], color);
    draw_line(v[1], v[2], color);
    draw_line(v[2], v[3], color);
    draw_line(v[3], v[0], color);

    draw_line(v[4], v[5], color);
    draw_line(v[5], v[6], color);
    draw_line(v[6], v[7], color);
    draw_line(v[7], v[4], color);

    draw_line(v[0], v[4], color);
    draw_line(v[1], v[5], color);
    draw_line(v[2], v[6], color);
    draw_line(v[3], v[7], color);
  }
}

void DebugProcessor::gather_aabb()
{
  auto entities = world().all_entities();
  std::vector<Vec3f> lines;
  lines.reserve(entities.size() * 24);
  const Vec3f color(rgb_to_vec3f(0xdf5a44));

  for (const sptr<Entity> &entity : world().all_entities()) {
    const BoundingBox &box = entity->aabb();
    const Mat4f &t = entity->transform();

    const Vec3f v[] = {
      transform_point(t, Vec3f(box.xmin, box.ymin, box.zmin)),
      transform_point(t, Vec3f(box.xmax, box.ymin, box.zmin)),
      transform_point(t, Vec3f(box.xmax, box.ymax, box.zmin)),
      transform_point(t, Vec3f(box.xmin, box.ymax, box.zmin)),
      transform_point(t, Vec3f(box.xmin, box.ymin, box.zmax)),
      transform_point(t, Vec3f(box.xmax, box.ymin, box.zmax)),
      transform_point(t, Vec3f(box.xmax, box.ymax, box.zmax)),
      transform_point(t, Vec3f(box.xmin, box.ymax, box.zmax)),
    };

    draw_line(v[0], v[1], color);
    draw_line(v[1], v[2], color);
    draw_line(v[2], v[3], color);
    draw_line(v[3], v[0], color);

    draw_line(v[4], v[5], color);
    draw_line(v[5], v[6], color);
    draw_line(v[6], v[7], color);
    draw_line(v[7], v[4], color);

    draw_line(v[0], v[4], color);
    draw_line(v[1], v[5], color);
    draw_line(v[2], v[6], color);
    draw_line(v[3], v[7], color);
  }
}

void DebugProcessor::gather_geometry_cache()
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
      const Slice<u32> indices = model->find_stream<u32>("indices");
      const Slice<Vec3f> vertices = to_slice(geometry_cache.vertices);
      const Mat4f t = entity->transform();

      for (u32 i = 0; i < indices.size(); i += 3) {
        const Vec3f color(rgb_to_vec3f(0x546475));
        draw_line(t, vertices[indices[i    ]], vertices[indices[i + 1]], color);
        draw_line(t, vertices[indices[i + 1]], vertices[indices[i + 2]], color);
        draw_line(t, vertices[indices[i + 2]], vertices[indices[i    ]], color);
      }
    }
  }

  core().video_service().enable_depth_test();
}

void DebugProcessor::draw_all()
{
  assert(my_debug_material != nullptr);

  if (!my_line_points.empty()) {
    Mesh mesh;
    VideoService &vs = core().video_service();
    uptr<VideoBuffer> vertex_buffer(new VideoBuffer(vs, VideoBufferUsage::STATIC_DRAW));
    uptr<VideoBuffer> color_buffer(new VideoBuffer(vs, VideoBufferUsage::STATIC_DRAW));
    vertex_buffer->set_bytes(my_line_points.data(), my_line_points.size() * sizeof(Vec3f));
    color_buffer->set_bytes(my_line_colors.data(), my_line_colors.size() * sizeof(Vec3f));
    mesh.vertex = std::move(vertex_buffer);
    mesh.color = std::move(color_buffer);

    Uniforms &u = vs.get_uniforms();
    u.color = Vec3f(1, 1, 1);
    u.transformations.model = Mat4f();
    u.model = Mat4f();
    u.mvp = u.transformations.model_view_projection();
    RenderContext context = { u, vs };

    my_debug_material->material().draw_mesh(context, mesh);
  }
}

}
