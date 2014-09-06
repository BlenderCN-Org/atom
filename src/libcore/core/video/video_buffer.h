#pragma once

#include "../noncopyable.h"
#include "../utils/gl_utils.h"
#include "../math/vec_array.h"
#include "video_service.h"

namespace atom {

/**
 * OpenGL buffer object, tato trieda v konstruktore vytvori obecny OpenGL buffer.
 * Umoznuje aj nahravanie dat.
 */
class VideoBuffer : NonCopyable {
  VideoService    &my_vs;
  u32              my_size;       ///< velkost dat v bytoch
  GLuint           my_gl_buffer;

public:
  explicit VideoBuffer(VideoService &vs);

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

  void set_data(const Vec2f *array, size_t count);

  void set_data(const Vec2fArray &array);

  void set_data(const Vec3f *array, size_t count);

  void set_data(const Vec3fArray &array);
};

}
