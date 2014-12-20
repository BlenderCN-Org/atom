#pragma once

#include "noncopyable.h"
#include "gl_utils.h"
#include "vec_array.h"
#include "video_service.h"

namespace atom {

enum class VideoBufferUsage {
  STATIC_DRAW,
  DYNAMIC_DRAW
};

/**
 * OpenGL buffer object, tato trieda v konstruktore vytvori obecny OpenGL buffer.
 * Umoznuje aj nahravanie dat.
 */
class VideoBuffer : NonCopyable {
  VideoService    &my_vs;
  u32              my_size;       ///< velkost dat v bytoch
  VideoBufferUsage my_usage;
  GLuint           my_gl_buffer;

public:
  explicit VideoBuffer(VideoService &vs, VideoBufferUsage usage);

  VideoBuffer(VideoBuffer &&buffer);

  ~VideoBuffer();

  void set_bytes(const void *data, u32 size);

  /**
   * Vrat velkost dat v bytoch.
   */
  u32 size() const
  {
    return my_size;
  }

  GLuint gl_buffer() const
  {
    return my_gl_buffer;
  }

  template<typename T>
  void set_data(const Slice<T> &data)
  {
    set_bytes(data.data(), data.raw_size());
  }

private:
  static GLenum get_gl_usage(VideoBufferUsage usage);
};

}
