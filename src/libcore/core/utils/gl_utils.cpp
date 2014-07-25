#include "gl_utils.h"

#ifdef __linux__
#  include <execinfo.h>
#endif

#include <cassert>
#include <GL/glew.h>
#include <GL/gl.h>
#include "../log.h"
#include "../string.h"
#include "utils.h"

namespace atom {

String gl_get_error_string(GLenum error)
{
  switch (error) {
    case GL_INVALID_ENUM:
      return "GL_INVALID_ENUM";
    case GL_INVALID_VALUE:
      return "GL_INVALID_VALUE";
    case GL_INVALID_OPERATION:
      return "GL_INVALID_OPERATION";
    case GL_OUT_OF_MEMORY:
      return "GL_OUT_OF MEMORY";
  }

  return String("UNKNOWN ") + to_string(error);
}

// Tato funkcia je pouzita iba v Debug verzii
#ifndef NDEBUG
void gl_check_error_with_message(const char *first, const char *second)
{
  GLenum error;
  while ((error = glGetError()) != GL_NO_ERROR) {
    log::warning("OpenGL error %s %s", gl_get_error_string(error).c_str(), first);
    log::warning("%s", second);

#ifdef __linux__
    log::warning("Backtrace");
    const int BACKTRACE_SIZE = 5;
    void * array[BACKTRACE_SIZE];
    int size = backtrace(array, BACKTRACE_SIZE);
    uptr<char *, void (*)(void *)> symbols(backtrace_symbols(array, size), free);
    /// @todo pridat regularny vyraz na oseknutie vypisu, aby sa vypisoval len nazov metody/funkcie bez offsetu...
    for (int i = 0; i < size; ++i)
      log::warning("  %s", symbols.get()[i]);
#else
    log::warning("Use Linux to print backtracke, Windows stacktrace is not supported yet");
#endif
  }
}

GLErrorGuard::GLErrorGuard(const char *function)
  : my_function(function)
{
  assert(function != nullptr);
  gl_check_error_with_message("before function/method", my_function);
}

GLErrorGuard::~GLErrorGuard()
{
  gl_check_error_with_message("in function method", my_function);
}

#endif

bool is_read_framebuffer_complete()
{
  return glCheckFramebufferStatus(GL_READ_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
}

bool is_draw_framebuffer_complete()
{
  return glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
}

}
