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
public:
  explicit VideoBuffer(VideoService &vs);

  VideoBuffer(VideoBuffer &&buffer);

  ~VideoBuffer();

  void set_data(const Vec2f *array, size_t count);

  void set_data(const Vec2fArray &array);

  void set_data(const Vec3f *array, size_t count);

  void set_data(const Vec3fArray &array);

  void set_data(const void *data, size_t size);

  /**
   * Vrat velkost dat v bytoch.
   */
  int size() const;

  GLuint gl_buffer() const
  {
    return my_gl_buffer;
  }

private:
  VideoService    &my_vs;
  size_t           my_size;       ///< velkost dat v bytoch
  GLuint           my_gl_buffer;
};

}
