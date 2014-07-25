#pragma once

#include "../corefwd.h"
#include "../system/resource_service.h"
#include "../constants.h"
#include "video_buffer.h"

namespace atom {

class DrawService {
public:
  DrawService(VideoService &vs, ResourceService &rs);

  ~DrawService();

  /**
   * Vykresli pixmapu na danu poziciu a v zadanych rozmeroch.
   * Pouziva transformation stack.
   *
   * @param vp odkaz na platny VideoService
   * @param x pociatok pixmapy
   * @param y pociatok pixmapy
   * @param w sirka pixmapy
   * @param h vyska pixmapy
   */

//  void draw_mesh_buffer(const Mesh &mesh_buffer);

  /**
   * Nakresli text.
   *
   * @param pos pociatocna pozicia (laveho horneho rohu)
   * @param height vyska textu
   */
//  void draw_text(const BitmapFont &font,
//    const Vec2f &pos,
//    float height,
//    const char *text,
//    const Vec4f &color = Vec4f(1, 1, 1, 1));

// momentalne implementuju transformacie cez transformation stack
//  void draw_lines(const Vec3fArray &vertices);
//  void draw_lines(const Vec3fArray &vertices, const IndexArray &indices);

  /**
   * Vykresli primitivu obdlznik.
   *
   * Vrcholy su v CCW poradi.
   */
  void draw_primitive_rect(
    const Vec3f vertices[4],
    const Vec2f uv[4]);

private:
  void init_cross();

public:
  VideoService &my_vs;

//    TextureBuffer        *my_lookup_buffer;

  GLuint      my_gl_framebuffer;
  GLuint      my_gl_uv_texture;
  VideoBuffer my_cross_vertices;
};

}
