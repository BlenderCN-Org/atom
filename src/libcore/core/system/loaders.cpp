#include "loaders.h"
#include "../video/material.h"
#include "../video/bitmap_font.h"
#include "../video/model.h"
#include "../video/mesh.h"
#include "../audio/sound.h"
#include "../audio/music.h"
#include "resource_service.h"

namespace atom {

//-----------------------------------------------------------------------------
//
// Loader interface
//
//-----------------------------------------------------------------------------

Loader::~Loader()
{
}

//-----------------------------------------------------------------------------
//
// Image Loader
//
//-----------------------------------------------------------------------------

ResourcePtr ImageLoader::create_resource(ResourceService &rs, const String &name)
{
  assert(!name.empty());

  String filename = get_image_filename(name);

  uptr<Image> image = Image::create_from_file(filename.c_str());

  if (image == nullptr) {
    log::warning("Can't load image \"%s\"", filename.c_str());
    return nullptr;
  }

  ImageResourcePtr resource = std::make_shared<ImageResource>();
  resource->set_data(std::move(image));
  resource->set_name(resource_name(name));
  resource->depend_on_file(filename.c_str());
  resource->set_loader(this);
  return resource;
}

void ImageLoader::reload_resource(ResourceService &rp, Resource &resource)
{
  StringArray tokens = split_resource_name(resource.name());

  if (tokens.size() > 1) {
    String filename = String(IMAGE_RESOURCE_DIR) + "/" + tokens[1] + ".png";
    auto image = Image::create_from_file(filename.c_str());
    dynamic_cast<ImageResource &>(resource).set_data(std::move(image));
  }
}

String ImageLoader::resource_name(const String &name)
{
  return String(RESOURCE_IMAGE_TAG) + RESOURCE_DELIMITER + name;
}

String ImageLoader::get_image_filename(const String &name)
{
  return String(IMAGE_RESOURCE_DIR) + "/" + name + ".png";
}

//-----------------------------------------------------------------------------
//
// Texture Loader
//
//-----------------------------------------------------------------------------

ResourcePtr TextureLoader::create_resource(ResourceService &rs, const String &name)
{
  ImageResourcePtr image = rs.get_image(name);

  if (image == nullptr) {
    log::error("Can't find image \"%s\"", name.c_str());
    return nullptr;
  }

  uptr<Texture> texture(new Texture(rs.video_service()));
  texture->init_from_image(image->image());
//      Texture::create_from_image(rp.video_service(), image->image());
  TextureResourcePtr resource = std::make_shared<TextureResource>();
  resource->set_name(String("texture:") + name);
  resource->depend_on_resource(image);
  resource->set_data(std::move(texture));
  resource->set_loader(this);
  return resource;
}

void TextureLoader::reload_resource(ResourceService &rs, Resource &resource)
{
  StringArray tokens = split_resource_name(resource.name());

  if (tokens.size() > 1) {
    uptr<Texture> texture(new Texture(rs.video_service()));
    texture->init_from_image(rs.get_image(tokens[1])->image());
    dynamic_cast<TextureResource &>(resource).set_data(std::move(texture));
  }
//  if (!my_texture_name.empty()) {
//    auto texture = Texture::create_from_image(rp.video_service(), *rp.get_image(my_texture_name));
//    resource.data().reset(texture);
//  }
}

//-----------------------------------------------------------------------------
//
// Shader Loader
//
//-----------------------------------------------------------------------------

TechniqueLoader::~TechniqueLoader()
{
}

ResourcePtr TechniqueLoader::create_resource(ResourceService &rs, const String &name)
{
  auto program = Technique::create(name);

  if (program == nullptr) {
    log::error("Can't create shader \"%s\"", name.c_str());
    return nullptr;
  }

  TechniqueResourcePtr resource = std::make_shared<TechniqueResource>();
  resource->set_name(make_resource_name(RESOURCE_SHADER_TAG, name));
  resource->set_data(std::move(program));
  resource->depend_on_files(get_shader_source_files(name));
  resource->set_loader(this);
  return resource;
}

StringArray TechniqueLoader::get_shader_source_files(const String &name)
{
  assert(!name.empty());
  StringArray sources;
  sources.push_back(String(SHADER_RESOURCE_DIR) + "/" + name + ".ps");
  sources.push_back(String(SHADER_RESOURCE_DIR) + "/" + name + ".vs");
  sources.push_back(String(SHADER_RESOURCE_DIR) + "/" + name + ".gs");
  return sources;
}

void TechniqueLoader::reload_resource(ResourceService &rs, Resource &resource)
{
  StringArray tokens = split_resource_name(resource.name());
  const String &name = tokens[1];
  auto program = Technique::create(name);
  // replace old technique only when the new one has been succesfully loaded
  if (program != nullptr) {
    dynamic_cast<TechniqueResource &>(resource).set_data(std::move(program));
  } else {
    log::warning("Can't reload technique \"%s\"", name.c_str());
  }
}

//-----------------------------------------------------------------------------
//
// Material Loader
//
//-----------------------------------------------------------------------------

void load_material_properties_from_json(const rapidjson::Value &obj, Material &material,
  ResourceService &rs, const String &filename)
{
  if (material.meta == nullptr || material.meta->fields == nullptr)
    return;

//  const unsigned count = material.meta->properties_count;
//  const MetaField *p = material.meta->properties;

  MetaFieldsEnumarator prop(*material.meta);
  const MetaField *p;

  while ((p = prop.next())) {
    utils::ReadResult result = utils::read_property_from_json(obj, *p, &material, rs);

    if (result != utils::ReadResult::OK)
      log::warning("Can't read property \"%s\" for material \"%s\"", p->name, filename.c_str());
  }

//  for (unsigned i = 0; i < count; ++i) {
//    if (!read_property_from_json(&json, *p, &material, rp))
//      log::warning("Can't read property \"%s\" for material \"%s\"", p->name, filename.c_str());
//    ++p;
//  }
}

uptr<Material> create_material(
  MaterialLoader &loader,
  const String &name,
  ResourceService &rs)
{
  String filename = MaterialLoader::get_material_filename(name);

  std::ifstream input(filename);

  if (!input.is_open()) {
    log::error("Can't load material file \"%s\"", filename.c_str());
    return nullptr;
  }

  utils::JsonInputStream stream(input);
  rapidjson::Document doc;
  doc.ParseStream<0>(stream);

  if (doc.HasParseError()) {
    log::error("Can't load parse material file \"%s\", error \"%s\"", filename.c_str(),
      doc.GetParseError());
    return nullptr;
  }

//  json_error_t error;

//  JsonPtr root(json_load_file(filename.c_str(), 0, &error), json_decref);

//  if (root == nullptr) {
//    log::error("Can't load material file \"%s\", line %i: %s", filename.c_str(), error.line, error.text);
//    return nullptr;
//  }

  if (!doc.HasMember("type")) {
    log::error("Material \"%s\" missing \"type\" value", filename.c_str());
    return nullptr;
  }

  const rapidjson::Value &material_type = doc["type"];

  if (!material_type.IsString()) {
    log::error("Invalid \"type\" for material \"%s\"", filename.c_str());
    return nullptr;
  }

  String material_name(material_type.GetString());

  uptr<Material> material;

  for (const MaterialCreator &info : loader.material_creators) {
    if (info.name == material_name) {
      material = info.create(rs);
      break;
    }
  }

  if (material == nullptr)
    return nullptr;

  // load json properties
  load_material_properties_from_json(doc, *material, rs, filename);
  return material;
}

ResourcePtr MaterialLoader::create_resource(ResourceService &rs, const String &name)
{
  auto material = create_material(*this, name, rs);

  if (material == nullptr) {
    log::error("Material \"%s\" is not implemented", name.c_str());
    return nullptr;
  }

  MaterialResourcePtr resource = std::make_shared<MaterialResource>();
  resource->set_name(make_resource_name(RESOURCE_MATERIAL_TAG, name));
  resource->set_data(std::move(material));
  resource->depend_on_file(get_material_filename(name));
  resource->set_loader(this);
  return resource;
}

void MaterialLoader::reload_resource(ResourceService &rs, Resource &resource)
{
  StringArray tokens = split_resource_name(resource.name());
  auto material = create_material(*this, tokens[1], rs);

  if (material == nullptr) {
    log::error("Can't reload material \"%s\"", resource.name().c_str());
    return;
  }

  dynamic_cast<MaterialResource &>(resource).set_data(std::move(material));
}

String MaterialLoader::get_material_filename(const String &name)
{
  return String(MATERIAL_DIR) + "/" + name + "." + MATERIAL_EXT;
}

MaterialLoader::MaterialLoader()
{
  material_creators.push_back(MaterialCreator("lines"    , LinesMaterial::create));
  material_creators.push_back(MaterialCreator("flat"     , FlatMaterial::create));
  material_creators.push_back(MaterialCreator("wireframe", WireframeMaterial::create));
  material_creators.push_back(MaterialCreator("phong" , PhongMaterial::create));
  material_creators.push_back(MaterialCreator("flat_skin"  , FlatSkinMaterial::create));
}

MaterialLoader::~MaterialLoader()
{
}

//-----------------------------------------------------------------------------
//
// Mesh Loader
//
//-----------------------------------------------------------------------------

ResourcePtr MeshLoader::create_resource(ResourceService &rs, const String &name)
{
  ModelResourcePtr model_resource = rs.get_model(name);

  if (model_resource == nullptr) {
    return nullptr;
  }

  uptr<Mesh> mesh(new Mesh());
  load_mesh_from_model(rs, model_resource->model(), *mesh);

  sptr<MeshResource> resource = std::make_shared<MeshResource>();
  resource->set_name(make_resource_name(RESOURCE_MESH_TAG, name));
  resource->depend_on_resource(model_resource);
  resource->set_data(std::move(mesh));
  resource->set_loader(this);
  return resource;
}

void MeshLoader::reload_resource(ResourceService &rs, Resource &resource)
{
  StringArray tokens = split_resource_name(resource.name());

  if (tokens.size() > 1) {
    ModelResourcePtr model_resource = rs.get_model(tokens[1]);

    if (model_resource == nullptr) {
      return;
    }

    uptr<Mesh> mesh(new Mesh());
    load_mesh_from_model(rs, model_resource->model(), *mesh);
    dynamic_cast<MeshResource &>(resource).set_data(std::move(mesh));
  }
}

void MeshLoader::load_mesh_from_model(ResourceService &rs, const Model &model, Mesh &mesh)
{
  const ElementArray *vertices = model.find_array("vertices", Type::F32);
  const ElementArray *normals = model.find_array("normals", Type::F32);
  const ElementArray *indices = model.find_array("indices", Type::U32);
  const ElementArray *bone_index = model.find_array("bone_index", Type::U32);
  const ElementArray *bone_weight = model.find_array("bone_weight", Type::F32);

  if (vertices != nullptr) {
    mesh.vertex.reset(new VideoBuffer(rs.video_service()));
    mesh.vertex->set_bytes(vertices->data.get(), vertices->size);
  }

  if (normals != nullptr) {
    mesh.normal.reset(new VideoBuffer(rs.video_service()));
    mesh.normal->set_bytes(normals->data.get(), normals->size);
  }

  if (indices != nullptr) {
    mesh.surface.reset(new VideoBuffer(rs.video_service()));
    mesh.surface->set_bytes(indices->data.get(), indices->size);
  }

  if (bone_weight != nullptr) {
    mesh.bone_weight.reset(new VideoBuffer(rs.video_service()));
    mesh.bone_weight->set_bytes(bone_weight->data.get(), bone_weight->size);
  }

  if (bone_index != nullptr) {
    mesh.bone_index.reset(new VideoBuffer(rs.video_service()));
    mesh.bone_index->set_bytes(bone_index->data.get(), bone_index->size);
  }
}

//-----------------------------------------------------------------------------
//
// Bitmap Font Loader
//
//-----------------------------------------------------------------------------

ResourcePtr BitmapFontLoader::create_resource(ResourceService &rs, const String &name)
{
  auto texture = rs.get_texture(name);

  if (texture == nullptr) {
    log::error("Can't find texture \"%s\"", name.c_str());
    return nullptr;
  }

  uptr<BitmapFont> bitmap_font = BitmapFont::create(rs, name);

//  TexturePtr texture = Texture::create_from_image(rp.video_service(), *image->image());
  auto resource = std::make_shared<BitmapFontResource>();
  resource->set_name(String("bitmap_font:") + name);
  resource->depend_on_resource(texture);
  resource->set_data(std::move(bitmap_font));
  resource->set_loader(this);
  return resource;
}

void BitmapFontLoader::reload_resource(ResourceService &rs, Resource &resource)
{
  StringArray tokens = split_resource_name(resource.name());

  if (tokens.size() > 1) {
    auto bitmap_font = BitmapFont::create(rs, tokens[1]);
    dynamic_cast<BitmapFontResource &>(resource).set_data(std::move(bitmap_font));
  }
}

//-----------------------------------------------------------------------------
//
// Sound Loader
//
//-----------------------------------------------------------------------------

ResourcePtr SoundLoader::create_resource(ResourceService &rs, const String &name)
{
  String filename = sound_filename(name);
  auto sound = Sound::create_from_file(filename.c_str());
  SoundResourcePtr resource = std::make_shared<SoundResource>();
  resource->set_name(String("sound:") + name);
  resource->depend_on_file(filename);
  resource->set_data(std::move(sound));
  resource->set_loader(this);
  return resource;
}

void SoundLoader::reload_resource(ResourceService &rs, Resource &resource)
{
  not_implemented();
}

String SoundLoader::sound_filename(
  const String &name)
{
  return String(SOUND_RESOURCE_DIR) + "/" + name + ".ogg";
}

//-----------------------------------------------------------------------------
//
// Loader
//
//-----------------------------------------------------------------------------

ResourcePtr MusicLoader::create_resource(
  ResourceService &rs,
  const String &name)
{
  String filename = music_file(name);
  auto music = Music::create_from_file(filename.c_str());
  MusicResourcePtr resource = std::make_shared<MusicResource>();
  resource->set_name(String("music:") + name);
  resource->depend_on_file(filename);
  resource->set_data(std::move(music));
  resource->set_loader(this);
  return resource;
}

void MusicLoader::reload_resource(ResourceService &rs, Resource &resource)
{
  not_implemented();
}

String MusicLoader::music_file(
  const String &name)
{
  return String(MUSIC_RESOURCE_DIR) + "/" + name + ".ogg";
}

}
