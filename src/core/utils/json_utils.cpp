#include "json_utils.h"
#include <cassert>
#include <cstring>
#include "core/log.h"
#include "core/system/resource_service.h"
#include "core/math/vec.h"
#include "core/video/video_service.h"

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

template<typename T>
bool read_vector(const rapidjson::Value &node, T &value)
{
  if (!node.IsArray() || node.Size() != T::SIZE) {
    return false;
  }

  T tmp;

  for (uint i = 0; i < node.Size(); ++i) {
    const rapidjson::Value &element = node[i];

    if (!element.IsNumber()) {
      return false;
    }

    tmp[i] = get_number_from_json<typename T::value_type>(element);
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

  auto texture_resource = rs.get_texture_resource(String(node.GetString()));

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

  auto shader_resource = rs.get_shader_resource(String(node.GetString()));

  if (shader_resource == nullptr)
    return false;

  shader = shader_resource;
  return true;
}

void write_float(float value, rapidjson::Value &node)
{
  node.SetDouble(value);
}

template<typename Allocator>
void write_vec2f(const Vec2f &value, rapidjson::Value &node, Allocator &allocator)
{
  node.SetArray();
  node.PushBack(value.x, allocator);
  node.PushBack(value.y, allocator);
}

template<typename Allocator>
void write_vec3f(const Vec3f &value, rapidjson::Value &node, Allocator &allocator)
{
  node.SetArray();
  node.PushBack(value.x, allocator);
  node.PushBack(value.y, allocator);
  node.PushBack(value.z, allocator);
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

    case Type::INT:
      parsed = read_number<int>(node, field_ref<int>(p, data));
      break;

    case Type::FLOAT:
      parsed = read_number<float>(node, field_ref<float>(p, data));
      break;

    case Type::STRING:
      parsed = read_string(node, field_ref<String>(p, data));
      break;

    case Type::VEC2F:
      parsed = read_vector<Vec2f>(node, field_ref<Vec2f>(p, data));
      break;

    case Type::VEC3F:
      parsed = read_vector<Vec3f>(node, field_ref<Vec3f>(p, data));
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

//      //  case Type::INT:
//      //  case Type::UINT:

    case Type::VEC2F:
      write_vec2f(field_ref<Vec2f>(field, data), node, doc.GetAllocator());
      obj.AddMember(field.name, node, doc.GetAllocator());
      return true;

    case Type::VEC3F:
      write_vec3f(field_ref<Vec3f>(field, data), node, doc.GetAllocator());
      obj.AddMember(field.name, node, doc.GetAllocator());
      return true;

    case Type::FLOAT:
      write_float(field_ref<float>(field, data), node);
      obj.AddMember(field.name, node, doc.GetAllocator());
      return true;

    case Type::STRING:
      write_string(field_ref<String>(field, data), node);
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
