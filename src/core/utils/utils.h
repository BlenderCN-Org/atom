#pragma once

#include "core/corefwd.h"
#include "core/types.h"
#include "core/log.h"
#include "core/stdvec.h"
#include "core/math/vec.h"
#include "core/math/mat.h"
#include "core/math/quaternion.h"

namespace atom {

const float PI = 3.14;
const u64 NANOSECONDS  = 1000000000UL;
const u64 MICROSECONDS = 1000000;
const u64 MILLISECONDS = 1000;

#ifdef NDEBUG

template<typename T, typename U>
T safe_cast(U value)
{
  return static_cast<T >(value);
}

template<typename T, typename U>
T& safe_cast(U &value)
{
  return static_cast<T &>(value);
}

#else

template<typename T, typename U>
T safe_cast(U value)
{
  return dynamic_cast<T >(value);
}

template<typename T, typename U>
T& safe_cast(U &value)
{
  return dynamic_cast<T &>(value);
}

#endif

i64 nano_time();

i64 micro_time();

i64 milli_time();

/**
 * Zisti velkost suboru.
 *
 * Na implementaciu je pouzita sablona pretoze musi podporovat ifstream, ofstream a fstream.
 */
u64 get_file_size(std::ifstream &file_stream);

inline float radians_to_degrees(float radians)
{
  return radians * 180 / PI;
}

//QQQ najst lepsi nazov
template<typename T>
T range(T min, T max, T value)
{
  return std::max(min, std::min(max, value));
}

#define to_size(value) (static_cast<std::size_t>(value))

template<typename T>
int to_int(T value)
{
  return static_cast<int>(value);
}

template<typename T>
float to_float(T value)
{
  return static_cast<float>(value);
}

String to_string(unsigned value);

String to_string(const Vec2f &v);

String to_string(const Vec3f &v);

String to_string(const Vec4f &v);

String to_string(const Quatf &q);

String to_string(const Mat4f &m);

template<typename T>
const char* to_cstr(const T &value)
{
  return to_string(value).c_str();
}

/**
 * Konvertuj texturu na obrazok.
 */
uptr<Image> to_image(VideoService &vs, Texture &texture);

inline Vec4f rgba_to_vec4f(u8 r, u8 g, u8 b, u8 a)
{
  return Vec4f(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f);
}

inline Vec3f rgb_to_vec3f(u8 r, u8 g, u8 b)
{
  return Vec3f(r / 255.0f, g / 255.0f, b / 255.0f);
}

/**
 * Get vector components of quaterion.
 */
template<typename T>
Vec3<T> to_point3(const Quaternion<T> &q)
{
  return Vec3<T>(q.x, q.y, q.z);
}

StringArray split_string(const String &str, char delim);

/**
 * Z dekorovaneho (mangled) mena ziskaj meno v citatelnej podobe (demangled).
 * Dekoduje mena len na gcc, na zvysnych kompilatoroch vracia povodne meno.
 *
 * @param mangled_name dekorovane meno
 * @return citatelne meno, alebo povodne ak sa neda dekodovat
 */
String demangle(const char *mangled_name);

/**
 * Mame hodnotu vyjadrenu za sekundu ale update funkcia pracuje po snimkoch (frame),
 * takze ju potrebujeme podelis snimkovacou frekvenciou. Nato slusi stf.
 */
inline f32 stf(f32 seconds)
{
  return seconds / FPS;
}

/**
 * Podobna ako stf, rozdiel je ze hodnota je vyjadrena nieco za minutu (x/m).
 */
inline f32 mtf(f32 minutes)
{
  return minutes / 60 / FPS;
}

namespace utils {

void vorbis_open_error(int code, const char *filename);

bool load_file_into_string(const char *filename, String &dst);
bool load_file_into_string(const String &filename, String &dst);

//void save_world_to_file(const sptr<World> &world, const String &filename);
//sptr<World> load_world_from_file(const String &filename, Core &core);

}

template<typename T>
const char* to_cstring(const T &v)
{
  return to_string(v).c_str();
}

namespace log {

/**
 * Print object function (using to_string() conversion function).
 * Conversion from object to string is done via to_string function.
 */
template<typename T>
inline void print(const T& value)
{
  info("%s\n", to_cstring(value));
}

}

void error(const char *format, ...);
}
