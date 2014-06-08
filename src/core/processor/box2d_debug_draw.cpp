#include "box2d_debug_draw.h"
#include "core/video/video_buffer.h"
#include "core/video/uniforms.h"
#include "core/math/camera.h"
#include "core/system/resource_service.h"

namespace atom {

const f32 Box2DDebugDraw::DEFAULT_Z = 0.0f;

Box2DDebugDraw::Box2DDebugDraw(VideoService &vs, ResourceService &rs)
  : my_vs(vs)
  , my_shader(rs.get_shader_resource("flat"))
{
  assert(my_shader.get() != nullptr && "FlatShader not fount");
  SetFlags(e_shapeBit | e_aabbBit);
}

Box2DDebugDraw::~Box2DDebugDraw()
{
}

void Box2DDebugDraw::reset()
{
  my_vertices.clear();
}

void Box2DDebugDraw::draw(const Camera &camera)
{
  VideoBuffer buffer(my_vs);
  buffer.set_data(my_vertices.data(), my_vertices.size());
  my_vs.set_draw_face(DrawFace::BOTH);

  Uniforms &u = my_vs.get_uniforms();
  u.color = Vec3f(1, 1, 1);

  u.transformations.view  = camera.view_matrix();
  u.transformations.projection = camera.projection_matrix();
  u.transformations.model = Mat4f::identity();

  Technique &program = my_shader->program();

  my_vs.bind_program(program);
  program.set_param("model_view_projection", u.transformations.model_view_projection());
  program.set_param("color", u.color);
//  my_shader->shader().apply_uniforms(my_vs, u);
  my_vs.bind_attribute(0, buffer, Type::VEC3F);

  my_vs.draw_arrays(GL_LINES, 0, my_vertices.size());
}

void Box2DDebugDraw::DrawPolygon(const b2Vec2 *vertices, int32 count, const b2Color &color)
{
  if (count <= 1) {
    log::warning("Invalid vertexCount %i", count);
    return;
  }

  for (int i = 1; i < count; ++i) {
    my_vertices.push_back(Vec3f(vertices[i - 1].x, vertices[i - 1].y, DEFAULT_Z));
    my_vertices.push_back(Vec3f(vertices[i].x, vertices[i].y, DEFAULT_Z));
  }
  // end segment
  my_vertices.push_back(Vec3f(vertices[count - 1].x, vertices[count -1].y, DEFAULT_Z));
  my_vertices.push_back(Vec3f(vertices[0].x, vertices[0].y, DEFAULT_Z));
}

void Box2DDebugDraw::DrawSolidPolygon(const b2Vec2 *vertices, int32 count, const b2Color &color)
{
  DrawPolygon(vertices, count, color);
}

void Box2DDebugDraw::DrawCircle(const b2Vec2 &center, float32 radius, const b2Color &color)
{
  not_implemented();
}

void Box2DDebugDraw::DrawSolidCircle(const b2Vec2 &center, float32 radius, const b2Vec2 &axis,
  const b2Color &color)
{
  DrawCircle(center, radius, color);
}

void Box2DDebugDraw::DrawSegment(const b2Vec2 &a, const b2Vec2 &b, const b2Color &color)
{
  my_vertices.push_back(Vec3f(a.x, a.y, DEFAULT_Z));
  my_vertices.push_back(Vec3f(b.x, b.y, DEFAULT_Z));
}

void Box2DDebugDraw::DrawTransform(const b2Transform &xf)
{
  not_implemented();
}

}
