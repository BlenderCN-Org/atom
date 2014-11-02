#pragma once

#include <algorithm>
#include "../foundation.h"
#include "../log.h"
#include "../stdvec.h"

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
  assert(dynamic_cast<T >(value) != nullptr);
  return dynamic_cast<T >(value);
}

template<typename T, typename U>
T& safe_cast(U &value)
{
  assert(dynamic_cast<T >(value) != nullptr);
  return dynamic_cast<T &>(value);
}

#endif

i64 nano_time();

i64 micro_time();

i64 milli_time();

/**
 * Convert string to TYPE (see implementation for concrete type names).
 * @return proper type or Type::UNKNOWN on error (or unsupported type)
 */
Type str_to_type(const String &str);


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

#define to_size(value) (static_cast<std::size_t>(value))

template<typename T>
int to_u32(T value)
{
  return static_cast<u32>(value);
}

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

/**
 * Convenient function to convert vector to slice
 */
template<typename T>
Slice<T> to_slice(const std::vector<T> &array)
{
  return Slice<T>(array.data(), array.size());
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

template<typename T>
void erase_remove(T &container, const typename T::value_type &elem)
{
  typename T::iterator removed = std::remove(container.begin(), container.end(), elem);
  container.erase(removed, container.end());
}

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
