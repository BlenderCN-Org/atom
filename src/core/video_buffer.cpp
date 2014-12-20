#include "video_buffer.h"

namespace atom {

VideoBuffer::VideoBuffer(VideoService &vs, VideoBufferUsage usage)
  : my_vs(vs)
  , my_size(0)
  , my_usage(usage)
  , my_gl_buffer(0)
{
  glGenBuffers(1, &my_gl_buffer);
}

VideoBuffer::VideoBuffer(VideoBuffer &&buffer)
  : my_vs(buffer.my_vs)
  , my_size(buffer.my_size)
  , my_usage(buffer.my_usage)
  , my_gl_buffer(buffer.my_gl_buffer)
{
  buffer.my_size = 0;
  buffer.my_gl_buffer = 0;
}

VideoBuffer::~VideoBuffer()
{
  glDeleteBuffers(1, &my_gl_buffer);
}

void VideoBuffer::set_bytes(const void *data, u32 size)
{
  assert(data != nullptr);
  assert(size > 0);
  // skopiruj data do OpenGL
  my_size = size;
  my_vs.bind_array_buffer(*this);
  glBufferData(GL_ARRAY_BUFFER, size, data, get_gl_usage(my_usage));
  my_vs.unbind_array_buffer();
}

GLenum VideoBuffer::get_gl_usage(VideoBufferUsage usage)
{
  switch (usage) {
    case VideoBufferUsage::DYNAMIC_DRAW:
      return GL_DYNAMIC_DRAW;

    case VideoBufferUsage::STATIC_DRAW:
      return GL_STATIC_DRAW;

    default:
      log::error("Unknown video buffer usage %i", usage);
      return GL_STATIC_DRAW;
  }
}

}
