#include "draw_service.h"

#include <string.h>
#include "../world/world.h"
#include "../system/config.h"
#include "../world/entity.h"
#include "video_buffer.h"
#include "bitmap_font.h"
#include "mesh.h"

using std::ofstream;
using std::swap;
using std::fill_n;

namespace atom {

DrawService::DrawService(VideoService &vs, ResourceService &rs)
  : my_vs(vs)
  , my_cross_vertices(vs)
{
  init_cross();
}

DrawService::~DrawService()
{
}

void DrawService::draw_primitive_rect(
  const Vec3f vertices[4],
  const Vec2f uv[4])
{
  Vec3f triangle_vertices[6] = {
    vertices[0], vertices[1], vertices[2],
    vertices[0], vertices[2], vertices[3]
  };

  Vec2f triangle_uv[6] = {
    uv[0], uv[1], uv[2],
    uv[0], uv[2], uv[3]
  };

  VideoBuffer vertex_buffer(my_vs);
  vertex_buffer.set_data(triangle_vertices, 6);

  VideoBuffer uv_buffer(my_vs);
  uv_buffer.set_data(triangle_uv, 6);

  my_vs.bind_attribute(0, vertex_buffer, Type::VEC3F);
  my_vs.bind_attribute(1, uv_buffer, Type::VEC2F);
  my_vs.draw_arrays(GL_TRIANGLES, 0, 6);
  my_vs.unbind_vertex_attribute(0);
  my_vs.unbind_vertex_attribute(1);

}

void DrawService::init_cross()
{
  const float radius = 1.0f;
  Vec3fArray cross_vertices;
  cross_vertices.push_back(Vec3f(-radius,  0.0f, 0.0f ));
  cross_vertices.push_back(Vec3f( radius,  0.0f, 0.0f ));
  cross_vertices.push_back(Vec3f(  0.0f,  radius, 0.0f ));
  cross_vertices.push_back(Vec3f(  0.0f, -radius, 0.0f ));
  cross_vertices.push_back(Vec3f(  0.0f, 0.0f,  radius ));
  cross_vertices.push_back(Vec3f(  0.0f, 0.0f, -radius ));

  my_cross_vertices.set_data(cross_vertices);
}

}
