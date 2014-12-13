#include "json_utils.h"
#include <cassert>
#include <cstring>
#include "../log.h"
#include "../system/resource_service.h"
#include "../video/video_service.h"
#include "../math/math.h"

namespace atom {
namespace utils {

template<>
int get_number_from_json(const rapidjson::Value &value)
{
  return value.GetInt();
}

template<>
u32 get_number_from_json(const rapidjson::Value &value)
{
  return value.GetUint();
}

template<>
float get_number_from_json(const rapidjson::Value &value)
{
  return value.GetDouble();
}

template<typename T>
bool read_element(const rapidjson::Value &node, T &value);

template<>
bool read_element(const rapidjson::Value &node, i32 &value)
{
  if (node.IsNumber()) {
    value = node.GetInt();
    return true;
  } else {
    return false;
  }
}

template<>
bool read_element(const rapidjson::Value &node, u32 &value)
{
  if (node.IsNumber()) {
    value = node.GetUint();
    return true;
  } else {
    return false;
  }
}

template<>
bool read_element(const rapidjson::Value &node, f32 &value)
{
  if (node.IsNumber()) {
    value = node.GetDouble();
    return true;
  } else {
    return false;
  }
}

template<typename T>
bool read_array(const rapidjson::Value &node, std::vector<T> &array)
{
  if (!node.IsArray()) {
    return false;
  }

  std::vector<T> tmp;

  for (u32 i = 0; i < node.Size(); ++i) {
    T element;
    if (read_element(node[i], element)) {
      tmp.push_back(element);
    } else {
      return false;
    }
  }

  array.swap(tmp);
  return true;
}

// force generate read_array for i32, u32, f32
template bool read_array<i32>(const rapidjson::Value &node, std::vector<i32> &value);
template bool read_array<u32>(const rapidjson::Value &node, std::vector<u32> &value);
template bool read_array<f32>(const rapidjson::Value &node, std::vector<f32> &value);

namespace {

template<typename T>
bool read_number(const rapidjson::Value &node, T &value)
{
  if (!node.IsNumber()) {
    return false;
  }

  value = get_number_from_json<T>(node);
  return true;
}

// pouziva sa pre nacitanie vec3f,... pole s pevnym poctom prvkov
template<typename T>
bool read_vector(const rapidjson::Value &node, T &value)
{
  if (!node.IsArray() || node.Size() != T::SIZE) {
    return false;
  }

  T tmp;

  for (u32 i = 0; i < node.Size(); ++i) {
    const rapidjson::Value &element = node[i];

    if (!element.IsNumber()) {
      return false;
    }

    tmp[i] = get_number_from_json<typename T::value_type>(element);
  }

  value = tmp;
  return true;
}

template<typename T>
bool read_matrix(const rapidjson::Value &node, T &value)
{
  if (!node.IsArray() || node.Size() != T::SIZE * T::SIZE) {
    return false;
  }

  T tmp;

  for (u32 col = 0; col < T::SIZE; ++col) {
    for (u32 row = 0; row < T::SIZE; ++row) {
      const rapidjson::Value &element = node[col * T::SIZE + row];

      if (!element.IsNumber()) {
        return false;
      }

      tmp[col][row] = get_number_from_json<typename T::ValueType>(element);
    }
  }

  value = tmp;
  return true;
}

bool read_bool(const rapidjson::Value &node, bool &value)
{
  if (node.IsTrue()) {
    value = true;
    return true;
  } else if (node.IsFalse()) {
    value = false;
    return true;
  }

  return false;
}

void write_bool(bool value, rapidjson::Value &node)
{
  node.SetBool(value);
}

void write_i32(i32 value, rapidjson::Value &node)
{
  node.SetInt(value);
}

void write_u32(u32 value, rapidjson::Value &node)
{
  node.SetUint(value);
}

bool read_string(const rapidjson::Value &node, String &value)
{
  if (!node.IsString()) {
    return false;
  }

  value = String(node.GetString());
  return true;
}

void write_string(const String &value, rapidjson::Value &node)
{
  node.SetString(value.data());
}

bool read_texture(const rapidjson::Value &node, TextureResourcePtr &texture, ResourceService &rs)
{
  if (!node.IsString()) {
    return false;
  }

  auto texture_resource = rs.get_texture(String(node.GetString()));

  if (texture_resource == nullptr)
    return false;

  texture = texture_resource;
  return true;
}

bool read_shader(const rapidjson::Value &node, TechniqueResourcePtr &shader, ResourceService &rs)
{
  if (!node.IsString()) {
    return false;
  }

  auto shader_resource = rs.get_technique(String(node.GetString()));

  if (shader_resource == nullptr)
    return false;

  shader = shader_resource;
  return true;
}

void write_float(float value, rapidjson::Value &node)
{
  node.SetDouble(value);
}

template<typename T, typename Allocator>
void write_vector(const T &v, rapidjson::Value &node, Allocator &allocator)
{
  node.SetArray();

  for (u32 i = 0; i < T::SIZE; ++i) {
    node.PushBack(v[i], allocator);
  }
}

template<typename T, typename Allocator>
void write_matrix(const T &m, rapidjson::Value &node, Allocator &allocator)
{
  node.SetArray();

  for (u32 col = 0; col < T::SIZE; ++col) {
    for (u32 row = 0; row < T::SIZE; ++row) {
      node.PushBack(m[col][row], allocator);
    }
  }
}

bool read_draw_face(const rapidjson::Value &node, DrawFace &face)
{
  if (!node.IsString()) {
    return false;
  }

  const char *value = node.GetString();

  if (!strcmp(value, "front")) {
    face = DrawFace::FRONT;
    return true;
  } else if (!strcmp(value, "back")) {
    face = DrawFace::BACK;
    return true;
  } else if (!strcmp(value, "both")) {
    face = DrawFace::BOTH;
    return true;
  }

  return false;
}

bool read_draw_type(const rapidjson::Value &node, DrawType &draw_type)
{
  if (!node.IsString()) {
    return false;
  }

  const char *value = node.GetString();

  if (!strcmp(value, "lines")) {
    draw_type = DrawType::LINES;
    return true;
  } else if (!strcmp(value, "triangles")) {
    draw_type = DrawType::TRIANGLES;
    return true;
  }

  return false;
}

bool read_fill_mode(const rapidjson::Value &node, FillMode &mode)
{
  if (!node.IsString()) {
    return false;
  }

  const char *value = node.GetString();

  if (!strcmp(value, "fill")) {
    mode = FillMode::FILL;
    return true;
  } else if (!strcmp(value, "line")) {
    mode = FillMode::LINE;
    return true;
  } else if (!strcmp(value, "point")) {
    mode = FillMode::POINT;
    return true;
  }

  return false;
}

/**
 * Read color in html notation e.g. "c3354e", "8da87b", ...
 */
bool read_color(const rapidjson::Value &node, Vec3f &value)
{
  if (!node.IsString()) {
    return false;
  }
  // there is no way how check successful conversion (errno is not sufficient)
  u32 color = strtol(node.GetString(), nullptr, 0);
  value = rgb_to_vec3f(color);
  return true;
}

}


bool read_vec2f(const rapidjson::Value &node, Vec2f &v)
{
  return read_vector(node, v);
}

/**
 * Read 3 component array or hexadecimal color value.
 */
bool read_vec3f(const rapidjson::Value &node, Vec3f &v)
{
  if (read_vector(node, v)) {
    return true;
  }
  return read_color(node, v);
}

bool read_vec4f(const rapidjson::Value &node, Vec4f &v)
{
  return read_vector(node, v);
}

ReadResult read_basic_property_from_json(const rapidjson::Value &obj, const MetaField &p, void *data)
{
  assert(obj.IsObject());

  if (!obj.HasMember(p.name)) {
    return ReadResult::MISS;
  }

  const rapidjson::Value &node = obj[p.name];

  bool parsed = false;

  switch (p.type) {
    case Type::BOOL:
      parsed = read_bool(node, field_ref<bool>(p, data));
      break;

    case Type::I32:
      parsed = read_number<i32>(node, field_ref<i32>(p, data));
      break;

    case Type::U32:
      parsed = read_number<u32>(node, field_ref<u32>(p, data));
      break;

    case Type::F32:
      parsed = read_number<f32>(node, field_ref<f32>(p, data));
      break;

    case Type::STRING:
      parsed = read_string(node, field_ref<String>(p, data));
      break;

    case Type::VEC2F:
      parsed = read_vec2f(node, field_ref<Vec2f>(p, data));
      break;

    case Type::VEC3F:
      parsed = read_vec3f(node, field_ref<Vec3f>(p, data));
      break;

    case Type::MAT4F:
      parsed = read_matrix<Mat4f>(node, field_ref<Mat4f>(p, data));
      break;

    default:
      log::error("This type is not handled");
      break;
  }

  return parsed ? ReadResult::OK : ReadResult::FAIL;
}

ReadResult read_property_from_json(const rapidjson::Value &obj, const MetaField &p, void *data,
  ResourceService &rs)
{
  assert(obj.IsObject());

  if (!obj.HasMember(p.name)) {
    return ReadResult::MISS;
  }

  const rapidjson::Value &node = obj[p.name];

  bool parsed = false;

  switch (p.type) {
    case Type::TEXTURE:
      parsed = read_texture(node, field_ref<TextureResourcePtr>(p, data), rs);
      break;

    case Type::SHADER:
      parsed = read_shader(node, field_ref<TechniqueResourcePtr>(p, data), rs);
      break;

    case Type::DRAW_FACE:
      parsed = read_draw_face(node, field_ref<DrawFace>(p, data));
      break;

    case Type::DRAW_TYPE:
      parsed = read_draw_type(node, field_ref<DrawType>(p, data));
      break;

    case Type::FILL_MODE:
      parsed = read_fill_mode(node, field_ref<FillMode>(p, data));
      break;

    default:
      return read_basic_property_from_json(obj, p, data);
  }

  return parsed ? ReadResult::OK : ReadResult::FAIL;
}

bool write_basic_property_to_json(rapidjson::Document &doc, rapidjson::Value &obj,
  const MetaField &field, void *data)
{
  assert(obj.IsObject());

  rapidjson::Value node;

  switch (field.type) {
    case Type::BOOL:
      write_bool(field_ref<bool>(field, data), node);
      obj.AddMember(field.name, node, doc.GetAllocator());
      return true;

    case Type::I32:
      write_i32(field_ref<i32>(field, data), node);
      obj.AddMember(field.name, node, doc.GetAllocator());
      return true;

    case Type::U32:
      write_u32(field_ref<u32>(field, data), node);
      obj.AddMember(field.name, node, doc.GetAllocator());
      return true;

    case Type::F32:
      write_float(field_ref<float>(field, data), node);
      obj.AddMember(field.name, node, doc.GetAllocator());
      return true;

    case Type::STRING:
      write_string(field_ref<String>(field, data), node);
      obj.AddMember(field.name, node, doc.GetAllocator());
      return true;

    case Type::VEC2F:
      write_vector<Vec2f>(field_ref<Vec2f>(field, data), node, doc.GetAllocator());
      obj.AddMember(field.name, node, doc.GetAllocator());
      return true;

    case Type::VEC3F:
      write_vector<Vec3f>(field_ref<Vec3f>(field, data), node, doc.GetAllocator());
      obj.AddMember(field.name, node, doc.GetAllocator());
      return true;

    case Type::MAT4F:
      write_matrix<Mat4f>(field_ref<Mat4f>(field, data), node, doc.GetAllocator());
      obj.AddMember(field.name, node, doc.GetAllocator());
      return true;

    default:
      log::error("This type is not handled");
      return false;
  }

  return false;
}

JsonInputStream::JsonInputStream(std::ifstream &input)
  : my_input(&input)
{
}

char JsonInputStream::Peek()
{
  return transform_eof(my_input->peek());
}

char JsonInputStream::Take()
{
  return transform_eof(my_input->get());
}

std::size_t JsonInputStream::Tell()
{
  return my_input->tellg();
}

void JsonInputStream::Put(char)
{
  error("JsonInputStream doesn't support writting");
}

char* JsonInputStream::PutBegin()
{
  error("JsonInputStream doesn't support writting");
  return nullptr;
}

std::size_t JsonInputStream::PutEnd(char *)
{
  error("JsonInputStream doesn't support writting");
  return 0;
}

char JsonInputStream::transform_eof(int ch)
{
  if (ch == std::char_traits<char>::eof())
    return 0;

  return ch;
}

JsonOutputStream::JsonOutputStream(std::ofstream &output)
  : my_output(&output)
{
}

std::size_t JsonOutputStream::Tell()
{
  return my_output->tellp();
}

void JsonOutputStream::Put(char c)
{
  my_output->put(c);
}

}
}
