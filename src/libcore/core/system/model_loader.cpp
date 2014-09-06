#include "model_loader.h"
#include "../utils/utils.h"
#include "../utils/json_utils.h"

namespace atom {

//-----------------------------------------------------------------------------
//
// Model Loader
//
//-----------------------------------------------------------------------------

bool load_model_element_array_from_json(const rapidjson::Value &node,
  ElementArray &array)
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
          array.data = uptr<u8[]>(new u8[array.size]);
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
          array.data = uptr<u8[]>(new u8[array.size]);
          memcpy(array.data.get(), buffer.data(), array.size);
          return true;
        }
      }
      break;

    default:
      log::warning("Unknown array type \"%s\"", type.GetString());
      break;
  }

  log::warning("%s: Something went wrong", ATOM_FUNC_NAME);
  return false;
}

bool load_model_arrays_from_json(const rapidjson::Value &arrays_node, Model &model)
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

//    log::info("Loading array %s", i->name.GetString());
    const rapidjson::Value &array_node = i->value;
    ElementArray array;

    if (!load_model_element_array_from_json(array_node, array)) {
      log::warning("Error while loading array \"%s\"", i->name.GetString());
      return false;
    }
    model.add_array(i->name.GetString(), array.type, std::move(array.data), array.size);
  }

  return true;
}


bool load_model_skeleton_from_json(const rapidjson::Value &json_skeleton, Model &model)
{
  assert(json_skeleton.IsObject() && "Skeleton node must be JSON object");
  const rapidjson::Value &json_bones = json_skeleton["bones"];

  if (json_bones.IsNull()) {
    log::warning("Skeleton node doesn't contain bones");
    return false;
  }

  if (!json_bones.IsObject()) {
    log::warning("Skeleton node bones is invalid");
    return false;
  }

  auto i = json_bones.MemberBegin();
  auto end = json_bones.MemberEnd();

  i32 count = end - i;
  model.bones.resize(count);

  for (; i != end; ++i) {
    const rapidjson::Value &value = i->value;
    const rapidjson::Value &json_index = value["index"];

    if (json_index.IsNull() || !json_index.IsInt()) {
      log::warning("Invalid bone index");
      return false;
    }

    i32 index = json_index.GetInt();

    if (index >= count) {
      log::warning("Too big bone index %i, max %i", index, count);
      return false;
    }

    DataBone &bone = model.bones[index];
    bone.name = i->name.GetString();

    if (!utils::read_vec3f(value["head"], bone.head)) {
      log::warning("Can't read bone head");
    }

    if (!utils::read_vec3f(value["tail"], bone.tail)) {
      log::warning("Can't read bone tail");
    }

    if (!utils::read_vec3f(value["head_local"], bone.local_head)) {
      log::warning("Can't read bone head local");
    }

    if (!utils::read_vec3f(value["tail_local"], bone.local_tail)) {
      log::warning("Can't read bone tail local");
    }

    if (!utils::read_vec3f(value["x"], bone.x)) {
      log::warning("Can't read bone x axis");
    }

    if (!utils::read_vec3f(value["y"], bone.y)) {
      log::warning("Can't read bone y axis");
    }

    if (!utils::read_vec3f(value["z"], bone.z)) {
      log::warning("Can't read bone z axis");
    }

    const rapidjson::Value &json_parent = value["parent"];
    bone.parent = json_parent.IsNull() ? -1 : json_parent.GetInt();
  }

  return true;
}

bool load_model_from_json(const rapidjson::Value &mesh_node, Model &model)
{
  if (!mesh_node.IsObject()) {
    return false;
  }

  if (!load_model_arrays_from_json(mesh_node["arrays"], model)) {
    return false;
  }
  if (mesh_node.HasMember("skeleton") &&
    !load_model_skeleton_from_json(mesh_node["skeleton"], model)) {
    return false;
  }

  return true;
}

bool load_model(const String &filename, Model &model)
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

  return load_model_from_json(doc["mesh"], model);
}

ResourcePtr ModelLoader::create_resource(ResourceService &rs, const String &name)
{
  String filename = get_model_filename(name);
  uptr<Model> model(new Model());

  if (!load_model(filename, *model)) {
    return nullptr;
  }

  sptr<ModelResource> resource = std::make_shared<ModelResource>();
  resource->set_name(String(RESOURCE_MODEL_TAG) + ":" + name);
  resource->depend_on_file(filename);
  resource->set_loader(this);
  resource->set_data(std::move(model));
  return resource;
}

void ModelLoader::reload_resource(ResourceService &rs, Resource &resource)
{
  StringArray tokens = split_resource_name(resource.name());

  if (tokens.size() > 1) {
    auto model = load_model(get_model_filename(tokens[1]));

    if (model != nullptr) {
      dynamic_cast<ModelResource &>(resource).set_data(std::move(model));
    }
  }
}

String ModelLoader::get_model_filename(const String &name)
{
  return String(MESH_RESOURCE_DIR) + "/" + name + "." + MESH_EXT;
}

uptr<Model> load_model(const String &filename)
{
  uptr<Model> mesh(new Model());

  if (load_model(filename, *mesh)) {
    return std::move(mesh);
  }

  return nullptr;
}

}
