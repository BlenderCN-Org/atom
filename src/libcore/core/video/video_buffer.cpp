#include "video_buffer.h"

namespace atom {

VideoBuffer::VideoBuffer(VideoService &vs)
  : my_vs(vs)
  , my_size(0)
  , my_gl_buffer(0)
{
  glGenBuffers(1, &my_gl_buffer);
}

VideoBuffer::VideoBuffer(VideoBuffer &&buffer)
  : my_vs(buffer.my_vs)
  , my_size(buffer.my_size)
  , my_gl_buffer(buffer.my_gl_buffer)
{
  buffer.my_size = 0;
  buffer.my_gl_buffer = 0;
}

VideoBuffer::~VideoBuffer()
{
  glDeleteBuffers(1, &my_gl_buffer);
}

void VideoBuffer::set_data(const Vec2f *array, size_t count)
{
  set_raw_data(reinterpret_cast<const void *>(array), count * sizeof(Vec2f));
}

void VideoBuffer::set_data(const Vec2fArray &array)
{
  set_data(array.data(), array.size());
}

void VideoBuffer::set_data(const Vec3f *array, size_t count)
{
  set_raw_data(reinterpret_cast<const void *>(array), count * sizeof(Vec3f));
}

void VideoBuffer::set_data(const Vec3fArray &array)
{
  set_data(array.data(), array.size());
}

void VideoBuffer::set_raw_data(const void *data, size_t size)
{
  assert(data != nullptr);
  assert(size > 0);
  // skopiruj data do OpenGL
  my_size = size;
  my_vs.bind_array_buffer(*this);
  glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
  my_vs.unbind_array_buffer();
}

int VideoBuffer::size() const
{
  return my_size;
}

}
