#include "gbuffer.h"

namespace atom {

GBuffer::GBuffer(VideoService &vs)
  : my_vs(vs)
  , my_is_ready(false)
  , my_depth_pass(vs)
  , my_render_pass(vs)
  , my_albedo(vs)
  , my_material(vs)
  , my_normal(vs)
  , my_depth_stencil(vs)
{
}

void GBuffer::set_resolution(int width, int height)
{
  my_albedo.init_as_texture_2d(PixelFormat::RGBA, width, height);
  my_normal.init_as_texture_2d(PixelFormat::RG16F, width, height);
  my_depth_stencil.init_as_texture_2d(PixelFormat::D24S8, width, height);

  // set attachments
  if (!my_is_ready) {
    my_render_pass.set_color_attachment(0, my_albedo);
    my_render_pass.set_color_attachment(1, my_normal);
    my_render_pass.set_depth_stencil_attachment(my_depth_stencil);
    my_is_ready = true;
  }

  if (!my_render_pass.is_valid()) {
    error("GBuffer Render framebuffer is not complete");
  }
}

Framebuffer& GBuffer::get_render_framebuffer()
{
  return my_render_pass;
}

void GBuffer::blit()
{
  f32 screen_width = Config::instance().get_screen_width();
  f32 screen_height = Config::instance().get_screen_height();

  my_vs.bind_read_framebuffer(my_render_pass);
  my_vs.set_read_buffer(0);
  glBlitFramebuffer(0, 0, my_albedo.width(), my_albedo.height(), 0, 0,
    screen_width, screen_height, GL_COLOR_BUFFER_BIT, GL_LINEAR);

  my_vs.set_read_buffer(1);
  glBlitFramebuffer(0, 0, my_albedo.width(), my_albedo.height(), screen_width * 3 / 4, 0,
    screen_width, screen_height / 4, GL_COLOR_BUFFER_BIT, GL_LINEAR);
}

}
