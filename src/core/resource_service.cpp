#include "resource_service.h"
#include <algorithm>
#include <functional>
#include "constants.h"
#include "log.h"
#include "texture.h"
#include "sound.h"
#include "music.h"
#include "mesh.h"
#include "bitmap_font.h"
#include "model_loader.h"
#include "loaders.h"
#include "config.h"

namespace atom {

struct ResourceLoaders {
  ImageLoader      image;
  TextureLoader    texture;
  MaterialLoader   material;
  TechniqueLoader  technique;
  ModelLoader      model;
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
  log_debug(DEBUG_RESOURCES, "Loading resource \"%s\"", resource_name.c_str());

  if (resource != nullptr) {
    rs.add_resource(resource);
    return std::static_pointer_cast<T>(resource);
  } else {
    log_error("Can't load %s resource \"%s\"", prefix.c_str(), name.c_str());
    return nullptr;
  }
}

void print_resources(const ResourceArray &resources)
{
  for (const ResourcePtr &resource : resources)
    log_info("%s (%i)", resource->name().c_str(), resource.use_count() - 1);
}

}

ResourceService::ResourceService(Core &core)
  : my_core(core)
{
  init_loaders();

  // watch data directory
  my_file_watch.watch_dir(MESH_RESOURCE_DIR);
  my_file_watch.watch_dir(IMAGE_RESOURCE_DIR);
  my_file_watch.watch_dir(DEFAULT_SHADER_DIR.c_str(), WatchMethod::MOVE_OR_CLOSE);
  my_file_watch.watch_dir(MATERIAL_DIR, WatchMethod::MOVE_OR_CLOSE);
}

ResourceService::~ResourceService()
{
  log_debug(DEBUG_RESOURCES, "Releaseing all resources");

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
    log_info("Resource cycle dependency detected, fix resources");
    log_info("Resources ----------------");
    print_resources(resources);
    log_info("Used ----------------");
    print_resources(used);
    log_info("Used end----------------");
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
  log_debug(DEBUG_RESOURCES, "Creating resource loaders");
  my_loaders.reset(new ResourceLoaders());
}

void ResourceService::quit_loaders()
{
  assert(my_loaders != nullptr);
  log_debug(DEBUG_RESOURCES, "Destroying resource loaders");
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
      log_debug(DEBUG_RESOURCES, "Unloading the resource \"%s\"", resource->name().c_str());
      i = my_resources.erase(i);
    } else {
      ++i;
    }
  }
}

void ResourceService::print()
{
  log_info("Managing these (unified) resources");
  for (const ResourcePtr &resource : my_resources)
    log_info("%s (%i)", resource->name().c_str(), resource.use_count() - 1);
}

ImageResourcePtr ResourceService::get_image(const String &name)
{
  return find_or_load_resource<ImageResource>(*this, name, RESOURCE_IMAGE_TAG, my_loaders->image);
}

TextureResourcePtr ResourceService::get_texture(const String &name)
{
  return find_or_load_resource<TextureResource>(*this, name, RESOURCE_TEXTURE_TAG, my_loaders->texture);
}

TechniqueResourcePtr ResourceService::get_technique(const String &name)
{
  return find_or_load_resource<TechniqueResource>(*this, name, RESOURCE_SHADER_TAG, my_loaders->technique);
}

MaterialResourcePtr ResourceService::get_material(const String &name)
{
  return find_or_load_resource<MaterialResource>(*this, name, RESOURCE_MATERIAL_TAG, my_loaders->material);
}

ModelResourcePtr ResourceService::get_model(const String &name)
{
  return find_or_load_resource<ModelResource>(*this, name, RESOURCE_MODEL_TAG, my_loaders->model);
}

MeshResourcePtr ResourceService::get_mesh(const String &name)
{
  return find_or_load_resource<MeshResource>(*this, name, RESOURCE_MESH_TAG, my_loaders->mesh);
}

BitmapFontResourcePtr ResourceService::get_bitmap_font(const String &name)
{
  return find_or_load_resource<BitmapFontResource>(*this, name, RESOURCE_BITMAP_FONT_TAG, my_loaders->bitmap_font);
}

SoundResourcePtr ResourceService::get_sound(const String &name)
{
  return find_or_load_resource<SoundResource>(*this, name, RESOURCE_SOUND_TAG, my_loaders->sound);
}

MusicResourcePtr ResourceService::get_music(const String &name)
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
            log_debug(DEBUG_RESOURCES, "Reloading the \"%s\" resource", resource->name().c_str());
            loader->reload_resource(rs, *resource);
            next_changes.push_back(resource->name());
          } else {
            log_debug(DEBUG_RESOURCES, "There is no loader for \"%s\"", resource->name().c_str());
          }
        }
      }
    }

    changes = next_changes;
  }
  while (!changes.empty());

  if (change_list.empty()) {
    log_warning("Refresh called but there are no changes");
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
