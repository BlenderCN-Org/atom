#pragma once

#include "../corefwd.h"
#include "resources.h"
#include "file_watch.h"

namespace atom {

typedef std::vector<ResourcePtr> ResourceArray;

/**
 * Tato trieda reprezentuje inteligentnu spravu zdrojov (textura, obrazok, zvuk, hudba, ...).
 */
class ResourceService : private NonCopyable {
public:
  ResourceService(Core &core);

  ~ResourceService();

  Core& core() const;

  VideoService& video_service() const;

  void init_loaders();

  void quit_loaders();

  void poll();

  /**
   * Uvolni vsetky nepouzite zdroje (vsetky nereferencovane obrazky, textury, zvuky, ...)
   */
  void garbage_collect();

  void print();

  ImageResourcePtr get_image(const String &name);

  TextureResourcePtr get_texture(const String &name);

  TechniqueResourcePtr get_technique(const String &name);

  MaterialResourcePtr get_material(const String &name);

  RawMeshResourcePtr get_raw_mesh(const String &name);

  MeshResourcePtr get_mesh(const String &name);

  BitmapFontResourcePtr get_bitmap_font(const String &name);

  SoundResourcePtr get_sound(const String &name);

  MusicResourcePtr get_music(const String &name);

  ResourcePtr find_resource(const String &resource_name);

  void add_resource(const ResourcePtr &resource);

private:
  void refresh(ResourceService &rs, const StringArray &change_list);
  void refresh(ResourceService &rs, const String &resource_name);

private:
  // private members
  Core                  &my_core;
  uptr<ResourceLoaders>  my_loaders;
  ResourceArray          my_resources;
  FileWatch              my_file_watch;
};

}
