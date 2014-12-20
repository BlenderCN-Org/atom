#include "utils.h"

#ifdef __GNUC__
#include <cxxabi.h>
#endif

#include <cstdarg>
#include <fstream>
#include <iomanip>
#include <string>
#include <sstream>
#include <chrono>
#include <vorbis/codec.h>
#include "image.h"
#include "texture.h"
#include "framebuffer.h"
#include "renderbuffer.h"
#include "exception.h"
#include "log.h"
#include "gl_utils.h"

namespace atom {

i64 nano_time()
{
  // predpokladame ze high_resolution_clock ma presnost na nanosekundy
  std::chrono::nanoseconds now = std::chrono::high_resolution_clock::now().time_since_epoch();
  return now.count();
  // ak high_resolution_clock nema danu presnost bude potrebne nasledovne pretypovanie
//  auto now = std::chrono::high_resolution_clock::now().time_since_epoch();
//  return std::chrono::duration_cast<std::chrono::nano>(now).count();
}

i64 micro_time()
{
  auto now = std::chrono::high_resolution_clock::now().time_since_epoch();
  return std::chrono::duration_cast<std::chrono::microseconds>(now).count();
}

i64 milli_time()
{
  auto now = std::chrono::high_resolution_clock::now().time_since_epoch();
  return std::chrono::duration_cast<std::chrono::milliseconds>(now).count();
}

Type str_to_type(const String &str)
{
  if (str == "string") {
    return Type::STRING;
  } else if (str == "bool") {
    return Type::BOOL;
  } else if (str == "i8") {
    return Type::I8;
  } else if (str == "u8") {
    return Type::U8;
  } else if (str == "i16") {
    return Type::I16;
  } else if (str == "u16") {
    return Type::U16;
  } else if (str == "u32") {
    return Type::U32;
  } else if (str == "i32") {
    return Type::I32;
  } else if (str == "i64") {
    return Type::I64;
  } else if (str == "u64") {
    return Type::U64;
  } else if (str == "f32") {
    return Type::F32;
  } else if (str == "f64") {
    return Type::F64;
  } else if (str == "vec2f") {
    return Type::VEC2F;
  } else if (str == "vec3f") {
    return Type::VEC3F;
  } else if (str == "vec4f") {
    return Type::VEC4F;
  } else {
    return Type::UNKNOWN;
  }
}


u64 get_file_size(std::ifstream &file_stream)
{
  assert(file_stream.is_open());
  if (!file_stream.is_open())
    return -1;

  std::streampos current = file_stream.tellg();
  file_stream.seekg(0, std::ios_base::end);
  std::streampos end = file_stream.tellg();
  file_stream.seekg(current);

  return end;
}

String to_string(unsigned value)
{
  std::stringstream output;
  output << value;
  return output.str();
}

String to_string(const Vec2f &v)
{
  std::stringstream output;
  output << "(" << v[0] << ", " << v[1] << ")";
  return output.str();
}

String to_string(const Vec3f &v)
{
  std::stringstream output;
  output << "(" << v.x << ", " << v.y << ", " << v.z << ")";
  return output.str();
}

String to_string(const Vec4f &v)
{
  std::stringstream output;
  output << "(" << v[0] << ", " << v[1] << ", " << v[2] << ", " << v[3] << ")";
  return output.str();
}

String to_string(const Quatf &q)
{
  std::stringstream output;
  output << "(w=" << q.w << ", x=" << q.x << ", y=" << q.y << ", z=" << q.z << ")";
  return output.str();
}

String to_string(const Mat4f &m)
{
  std::stringstream output;
  output << std::fixed << std::setprecision(4);

  for (unsigned i = 0; i < 4; ++i) {
    output << m(0, i) << ", " <<
              m(1, i) << ", " <<
              m(2, i) << ", " <<
              m(3, i) << std::endl;
  }
  return output.str();
}

String demangle(const char *mangled_name)
{
  assert(mangled_name != nullptr);

#ifdef __GNUC__
  size_t length;
  int status;
  // pouzi free na uvolnenie pamate
  uptr<char, void(*)(void*)> demangled_name(
    abi::__cxa_demangle(mangled_name, nullptr, &length, &status), ::free);

  switch (status) {
    case 0:
      return String(demangled_name.get());

    case -1:
      log::warning("Memory allocation failed in demangle function");
      break;

    case -2:
      log::warning("This is not a valid C++ mangled name %s", mangled_name);
      break;

    case -3:
      log::warning("Invalid arguments for abi::__cxx_demangle");
      break;
  }
#endif

  return String(mangled_name);
}

StringArray split_string(const String &str, char delim)
{
  StringArray tokens;
  String::size_type s = 0;
  String::size_type e = 0;
  // najdi vsetky casti ukoncene oddelovacom
  while ((e = str.find(delim, s)) != String::npos) {
    int count = e - s;
    tokens.push_back(str.substr(s, count));
    s = e + 1;
  }
  // posledny podretazec nemusi byt ukonceni oddelovacom
  if (s <= str.size()) {
    tokens.push_back(str.substr(s));
  }

  return tokens;
}

namespace utils {

void vorbis_open_error(int code, const char *filename)
{
  const char *error_message;

  switch (code) {
    case OV_EREAD:
      error_message = "Can't read file";
      break;

    case OV_ENOTVORBIS:
      error_message = "This is not a vorbis ogg file";
      break;

    case OV_EVERSION:
      error_message = "Vorbis version mismatch";
      break;

    case OV_EBADHEADER:
      error_message = "Invalid Vorbis bitstream header";
      break;

    case OV_EFAULT:
      error_message = "Internal logic vault (maybe bug or heap/stack corruption)";
      break;

    default:
      error_message = "Unknown error";
      break;
  };

  error("%s (%i) \"%s\"", error_message, code, filename);
}

bool load_file_into_string(const char *filename, String &dst)
{
  assert(filename != nullptr);

  if (filename == nullptr)
    return false;

  std::ifstream input(filename);

  if (!input.is_open()) {
    return false;
  }

  int length = get_file_size(input);

  if (length < 0) {
    error("Getting file size error \"%s\"", filename);
    return false;
  } else if (length == 0) {
    return false;
  }

  /// @todo otestovat ci length nema byt o 1 vacsie
  dst.erase();
  dst.reserve(length);
  dst.append(std::istreambuf_iterator<char>(input), std::istreambuf_iterator<char>());

  return true;
}

bool load_file_into_string(const String &filename, String &dst)
{
  return load_file_into_string(filename.c_str(), dst);
}

uptr<Image> to_image(VideoService &vs, Texture &texture)
{
  GL_ERROR_GUARD;
  if (texture.type() != TextureType::TEXTURE_2D) {
    error("Only TEXTURE_2D type is supported/can be converted into image");
    return nullptr;
  }

  uptr<Image> image(new Image(texture.format(), texture.width(), texture.height()));

  vs.bind_texture(0, texture);
  glGetTexImage(GL_TEXTURE_2D, 0, texture.get_gl_data_format(), texture.get_gl_data_type(),
      image->pixels());
  vs.unbind_texture(0);
  return image;
}

}

void error(const char *format, ...)
{
  const size_t BUFFER_SIZE = 4096;
  char buffer[BUFFER_SIZE];
  va_list ap;
  va_start(ap, format);
  vsnprintf(buffer, BUFFER_SIZE, format, ap);
  log::error(buffer);
  va_end(ap);
  throw Exception(buffer);
}

}
