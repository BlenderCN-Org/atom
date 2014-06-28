#include "resource_service.h"
#include <algorithm>
#include <functional>
#include "core/system/loaders.h"
#include "core/system/raw_mesh_loader.h"
#include "core/constants.h"
#include "core/system/config.h"
#include "core/log.h"
#include "core/video/texture.h"
#include "core/audio/sound.h"
#include "core/audio/music.h"
#include "core/video/mesh.h"
#include "core/video/bitmap_font.h"

namespace atom {

struct ResourceLoaders {
  ImageLoader      image;
  TextureLoader    texture;
  MaterialLoader   material;
  ShaderLoader     shader;
  RawMeshLoader    raw_mesh;
  MeshLoader       mesh;
  BitmapFontLoader bitmap_font;
  SoundLoader      sound;
  MusicLoader      music;
};

namespace {

template<typename T, typename L>
sptr<T> find_or_load_resource(ResourceService &rs, const String &name, const String &prefix,
  L &loader)
{
  assert(!name.empty());
  String resource_name = prefix + ":" + name;

  ResourcePtr found = rs.find_resource(resource_name);

  if (found != nullptr) {
    return std::static_pointer_cast<T>(found);
  }

  ResourcePtr resource = loader.create_resource(rs, name);
  log::debug(DEBUG_RESOURCES, "Loading resource \"%s\"", resource_name.c_str());

  if (resource != nullptr) {
    rs.add_resource(resource);
    return std::static_pointer_cast<T>(resource);
  } else {
    log::error("Can't load %s resource \"%s\"", prefix.c_str(), name.c_str());
    return nullptr;
  }
}

void print_resources(const ResourceArray &resources)
{
  for (const ResourcePtr &resource : resources)
    log::info("%s (%i)", resource->name().c_str(), resource.use_count() - 1);
}

}

ResourceService::ResourceService(Core &core)
  : my_core(core)
{
  init_loaders();

  // watch data directory
  my_file_watch.watch_dir(MESH_RESOURCE_DIR);
  my_file_watch.watch_dir(IMAGE_RESOURCE_DIR);
  my_file_watch.watch_dir(DEFAULT_SHADER_DIR.c_str(), WatchMethod::MOVE);
  my_file_watch.watch_dir(MATERIAL_DIR, WatchMethod::MOVE);
}

ResourceService::~ResourceService()
{
  log::debug(DEBUG_RESOURCES, "Releaseing all resources");

  ResourceArray resources = std::move(my_resources);
  ResourceArray used;

  bool cycle = false;

  // resource dependency cycle detection
  do {
    for (const ResourcePtr &r : resources) {
      if (r.use_count() > 1) {
        used.push_back(r);  // this resource will be release in the next round
      }
    }

    cycle = used.size() == resources.size() && !used.empty();

    resources.swap(used);
    used.clear();
  } while (!resources.empty() && !cycle);

  if (cycle) {
    log::info("Resource cycle dependency detected, fix resources");
    log::info("Resources ----------------");
    print_resources(resources);
    log::info("Used ----------------");
    print_resources(used);
    log::info("Used end----------------");
  }

  quit_loaders();
}

Core& ResourceService::core() const
{
  return my_core;
}

VideoService& ResourceService::video_service() const
{
  return my_core.video_service();
}

void ResourceService::init_loaders()
{
  assert(my_loaders == nullptr);
  log::debug(DEBUG_RESOURCES, "Creating resource loaders");
  my_loaders.reset(new ResourceLoaders());
}

void ResourceService::quit_loaders()
{
  assert(my_loaders != nullptr);
  log::debug(DEBUG_RESOURCES, "Destroying resource loaders");
  my_loaders.reset();
}

void ResourceService::poll()
{
  my_file_watch.poll();
  StringArray change_list;

  for (const String &filename : my_file_watch.change_list())
    change_list.push_back(String("file:") + filename);

  if (!change_list.empty())
    refresh(*this, change_list);
}

void ResourceService::garbage_collect()
{
  auto i = my_resources.begin();

  while (i != my_resources.end()) {
    const ResourcePtr &resource = *i;
    if (resource.use_count() == 1) {
      log::debug(DEBUG_RESOURCES, "Unloading the resource \"%s\"", resource->name().c_str());
      i = my_resources.erase(i);
    } else {
      ++i;
    }
  }
}

void ResourceService::print()
{
  log::info("Managing these (unified) resources");
  for (const ResourcePtr &resource : my_resources)
    log::info("%s (%i)", resource->name().c_str(), resource.use_count() - 1);
}

ImageResourcePtr ResourceService::get_image_resource(const String &name)
{
  return find_or_load_resource<ImageResource>(*this, name, RESOURCE_IMAGE_TAG, my_loaders->image);
}

TextureResourcePtr ResourceService::get_texture_resource(const String &name)
{
  return find_or_load_resource<TextureResource>(*this, name, RESOURCE_TEXTURE_TAG, my_loaders->texture);
}

TechniqueResourcePtr ResourceService::get_shader_resource(const String &name)
{
  return find_or_load_resource<TechniqueResource>(*this, name, RESOURCE_SHADER_TAG, my_loaders->shader);
}

MaterialResourcePtr ResourceService::get_material_resource(const String &name)
{
  return find_or_load_resource<MaterialResource>(*this, name, RESOURCE_MATERIAL_TAG, my_loaders->material);
}

RawMeshResourcePtr ResourceService::get_raw_mesh_resource(const String &name)
{
  return find_or_load_resource<RawMeshResource>(*this, name, RESOURCE_MODEL_TAG, my_loaders->raw_mesh);
}

MeshResourcePtr ResourceService::get_mesh_resource(const String &name)
{
  return find_or_load_resource<MeshResource>(*this, name, RESOURCE_MESH_TAG, my_loaders->mesh);
}

BitmapFontResourcePtr ResourceService::get_bitmap_font_resource(const String &name)
{
  return find_or_load_resource<BitmapFontResource>(*this, name, RESOURCE_BITMAP_FONT_TAG, my_loaders->bitmap_font);
}

SoundResourcePtr ResourceService::get_sound_resource(const String &name)
{
  return find_or_load_resource<SoundResource>(*this, name, RESOURCE_SOUND_TAG, my_loaders->sound);
}

MusicResourcePtr ResourceService::get_music_resource(const String &name)
{
  return find_or_load_resource<MusicResource>(*this, name, RESOURCE_MUSIC_TAG, my_loaders->music);
}

ResourcePtr ResourceService::find_resource(const String &resource_name)
{
  assert(!resource_name.empty());

  auto found = std::find_if(my_resources.begin(), my_resources.end(),
    [&resource_name](const ResourcePtr &resource) -> bool
    { return resource->name() == resource_name; });

  return found != my_resources.end() ? *found : nullptr;
}

void ResourceService::add_resource(const ResourcePtr &resource)
{
  assert(resource != nullptr);
  assert(!resource->name().empty());
  assert(find_resource(resource->name()) == nullptr && "This resource already exists");
  my_resources.push_back(resource);
}

void ResourceService::refresh(ResourceService &rs, const StringArray &change_list)
{
  StringArray changes(change_list);

  do {
    StringArray next_changes;
    // pouzi kopiu, pretoze zoznam zdrojov sa moze pri updatovani zmenit (zvacsit)
    std::vector<ResourcePtr> resources(rs.my_resources);

    std::sort(changes.begin(), changes.end());
    std::unique(changes.begin(), changes.end());

    for (const String &change : changes) {
      for (const ResourcePtr &resource : resources) {
        const auto &sources = resource->sources();
        if (find(sources.begin(), sources.end(), change) != sources.end()) {
          Loader *loader = resource->loader();

          if (loader != nullptr) {
            log::debug(DEBUG_RESOURCES, "Reloading the \"%s\" resource", resource->name().c_str());
            loader->reload_resource(rs, *resource);
            next_changes.push_back(resource->name());
          } else {
            log::debug(DEBUG_RESOURCES, "There is no loader for \"%s\"", resource->name().c_str());
          }
        }
      }
    }

    changes = next_changes;
  }
  while (!changes.empty());

  if (change_list.empty()) {
    log::warning("Refresh called but there are no changes");
    return;
  }
}

void ResourceService::refresh(ResourceService &rs, const String &resource_name)
{
  StringArray change_list;
  change_list.push_back(resource_name);
  refresh(rs, change_list);
}

}
