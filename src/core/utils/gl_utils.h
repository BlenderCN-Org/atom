#pragma once

#include <GL/glew.h>
#include <GL/gl.h>
#include "core/math/vec.h"
#include "core/math/mat.h"

namespace atom {

// Debug verzia obsahuje podrobnu kontrolu OpenGL chybovych stavov
// funkcia gl_check_error by sa mala volat po kazdom volani do OpenGL
#ifndef NDEBUG

/**
 * Tato trieda sluzi na zachytenie OpenGL chybovych stavov a to pohodlnym sposobom.
 *
 * Pouziva sa prostrednictvom makra GL_ERROR_GUARD umiestnenym na zaciatok funckie/metody ktora vola OpenGL prikazy.
 * Zachytava a vypisuje OpenGL chyby pri vstupe do funkcie (presnejsie v mieste kde je pouzity GL_ERROR_GUARD) a pri opustani funckie.
 * Okrem vypisu chyby vypise ak backtrace pre lepsiu orientaciu v kontexte.
 */
class GLErrorGuard {
public:
  explicit GLErrorGuard(const char *function);
  ~GLErrorGuard();

private:
  const char *my_function;
};

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define GL_CHECK_ERROR gl_check_error_with_message("", "at line " TOSTRING(__LINE__) ", file " __FILE__)
#define GL_ERROR_GUARD GLErrorGuard gl_error_guard(MM_FUNC_NAME)

#else   // Release verzia neobsahuje detailnu kontrolu OpenGL chybovych stavov

// prazdna definicia GL_CHECK_ERROR v RELEASE verzii
#define GL_CHECK_ERROR

// prazdna definicia GL_ERROR_GUARD v RELEASE verzii
#define GL_ERROR_GUARD

#endif // NDEBUG
void gl_check_error_with_message(const char *first, const char *second);

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define gl_get_uniform_location(program, name) gl_get_uniform_location_with_message(program, name,  __FILE__ ":" TOSTRING(__LINE__))
#ifndef NDEBUG
/**
 * Bezpecnejsia verzia glGetUniformLocation - testuje vstupne parametre
 * a navratovu hodnotu funkcie glGetUniformLocation (pri chybe skonci
 * program asser-cnou chybou.
 * Pri nenajdeni danej hodnoty skonci chybou - plati pre Debug rezim.
 * Pri Release rezime sa chova presne rovnako ako glGetUniformLocation.
 */

GLint gl_get_uniform_location_with_message(GLuint program, const GLchar *name,
    const char *message);
#else // Release verzia - nema ziadne bezpecnostne opatrenia
inline GLint gl_get_uniform_location_with_message(GLuint program, const GLchar *name, const char *message)
{
  return glGetUniformLocation(program, name);
}

#endif

bool is_read_framebuffer_complete();

bool is_draw_framebuffer_complete();

//
// Konverzne funkcie
//

namespace utils {

inline const GLfloat* mat3f_to_gl(const Mat3f &m)
{
  return (GLfloat *) m.data;
}

inline const GLfloat* mat4f_to_gl(const Mat4f &m)
{
  return (GLfloat *) m.data;
}

inline const GLfloat* vec2f_to_gl(const Vec2f &v)
{
  return (GLfloat *) v.data;
}

//inline const GLfloat* vector3_to_gl(const Vec3 &v)
//{
//  return (GLfloat *) v.get_data();
//}

inline const GLfloat* vec4f_to_gl(const Vec4f &v)
{
  return (GLfloat *) v.data;
}

}
}
