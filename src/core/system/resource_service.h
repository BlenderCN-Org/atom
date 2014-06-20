#pragma once

#include "core/corefwd.h"
#include "core/system/resources.h"
#include "core/system/file_watch.h"

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

  ImageResourcePtr get_image_resource(const String &name);

  TextureResourcePtr get_texture_resource(const String &name);

  TechniqueResourcePtr get_shader_resource(const String &name);

  MaterialResourcePtr get_material_resource(const String &name);

  RawMeshResourcePtr get_raw_mesh_resource(const String &name);

  MeshResourcePtr get_mesh_resource(const String &name);

  BitmapFontResourcePtr get_bitmap_font_resource(const String &name);

  SoundResourcePtr get_sound_resource(const String &name);

  MusicResourcePtr get_music_resource(const String &name);

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
