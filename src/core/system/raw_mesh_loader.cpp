#include "raw_mesh_loader.h"
#include "core/utils/utils.h"
#include "core/utils/json_utils.h"

namespace atom {

//-----------------------------------------------------------------------------
//
// RawMesh Loader
//
//-----------------------------------------------------------------------------

bool load_raw_mesh_element_array_from_json(const rapidjson::Value &node, ElementArray &array)
{
  if (!node.IsObject()) {
    return false;
  }

  const rapidjson::Value &type = node["type"];
  const rapidjson::Value &data = node["data"];

  if (!type.IsString()) {
    log::error("\"type\" element is string");
    return false;
  }

  if (!data.IsArray()) {
    log::error("\"data\" element is array");
    return false;
  }

  Type data_type = str_to_type(type.GetString());

  if (data_type == Type::UNKNOWN) {
    return false;
  }

  switch (data_type) {
    case Type::U32:
      {
        std::vector<u32> buffer;
        if (utils::read_array(data, buffer) && !buffer.empty()) {
          array.size = buffer.size() * sizeof(u32);
          array.type = Type::U32;
          array.data = uptr<u8>(new u8[array.size]);
          log::info("Array size %i", array.size);
          memcpy(array.data.get(), buffer.data(), array.size);
          return true;
        }
      }
      break;

    case Type::F32:
      {
        std::vector<f32> buffer;
        if (utils::read_array(data, buffer) && !buffer.empty()) {
          array.size = buffer.size() * sizeof(f32);
          array.type = Type::F32;
          array.data = uptr<u8>(new u8[array.size]);
          memcpy(array.data.get(), buffer.data(), array.size);
          return true;
        }
      }
      break;

    default:
      break;
  }

  return false;
}

bool load_raw_mesh_arrays_from_json(const rapidjson::Value &arrays_node, RawMesh &model)
{
  if (!arrays_node.IsObject()) {
    return false;
  }

  auto i = arrays_node.MemberBegin();
  auto end = arrays_node.MemberEnd();

  for (; i != end; ++i) {
    if (!i->name.IsString()) {
      return false;
    }

    log::info("Loading array %s", i->name.GetString());
    const rapidjson::Value &array_node = i->value;
    ElementArray array;

    if (!load_raw_mesh_element_array_from_json(array_node, array)) {
      return false;
    }
    model.add_array(i->name.GetString(), array.type, std::move(array.data), array.size);
  }

  return true;
}

bool load_raw_mesh_from_json(const rapidjson::Value &mesh_node, RawMesh &model)
{
  if (!mesh_node.IsObject()) {
    return false;
  }

  return load_raw_mesh_arrays_from_json(mesh_node["arrays"], model);
}

bool load_model(const String &filename, RawMesh &model)
{
  std::ifstream input(filename);

  if (!input.is_open()) {
    return false;
  }

  utils::JsonInputStream stream(input);
  rapidjson::Document doc;
  doc.ParseStream<0>(stream);

  if (!doc.IsObject()) {
    return false;
  }

  return load_raw_mesh_from_json(doc["mesh"], model);
}

ResourcePtr RawMeshLoader::create_resource(ResourceService &rs, const String &name)
{
  String filename = get_model_filename(name);
  uptr<RawMesh> model(new RawMesh());

  if (!load_model(filename, *model)) {
    return nullptr;
  }

  sptr<RawMeshResource> resource = std::make_shared<RawMeshResource>();
  resource->set_name(String(RESOURCE_MODEL_TAG) + ":" + name);
  resource->depend_on_file(filename);
  resource->set_loader(this);
  resource->set_data(std::move(model));
  return resource;
}

void RawMeshLoader::reload_resource(ResourceService &rs, Resource &resource)
{
  StringArray tokens = split_resource_name(resource.name());

  if (tokens.size() > 1) {
    auto model = load_model(get_model_filename(tokens[1]));

    if (model != nullptr) {
      dynamic_cast<RawMeshResource &>(resource).set_data(std::move(model));
    }
  }
}

String RawMeshLoader::get_model_filename(const String &name)
{
  return String(MESH_RESOURCE_DIR) + "/" + name + "." + MESH_EXT;
}

uptr<RawMesh> load_model(const String &filename)
{
  uptr<RawMesh> mesh(new RawMesh());

  if (load_model(filename, *mesh)) {
    return std::move(mesh);
  }

  return nullptr;
}

}
