#pragma once

#include <map>
#include <rapidjson/document.h>
#include "core/corefwd.h"
#include "core/noncopyable.h"
#include "core/string.h"
#include "core/system/resources.h"
#include "core/utils/json_utils.h"
#include "core/video/raw_mesh.h"
#include "core/log.h"

namespace atom {

/**
 * Resource loader interface.
 */
class Loader : NonCopyable {
public:
  virtual ~Loader();

  virtual ResourcePtr create_resource(ResourceService &rs, const String &name) = 0;

  virtual void reload_resource(ResourceService &rs, Resource &resource) = 0;
};

//-----------------------------------------------------------------------------
//
// Image Loader
//
//-----------------------------------------------------------------------------

class ImageLoader : public Loader {
public:
  ResourcePtr create_resource(ResourceService &rs, const String &name) override;

  void reload_resource(ResourceService &rs, Resource &resource) override;

  String resource_name(const String &name);

  static String get_image_filename(const String &name);
};

//-----------------------------------------------------------------------------
//
// Texture Loader
//
//-----------------------------------------------------------------------------

class TextureLoader : public Loader {
public:
  ResourcePtr create_resource(ResourceService &rs, const String &name) override;

  void reload_resource(ResourceService &rs, Resource &resource) override;
};

//-----------------------------------------------------------------------------
//
// Shader Loader
//
//-----------------------------------------------------------------------------

class ShaderLoader : public Loader {
public:
  typedef uptr<Technique>(*ShaderCreator)();

  ~ShaderLoader();

  ResourcePtr create_resource(ResourceService &rs, const String &name) override;

  void reload_resource(ResourceService &rs, Resource &resource) override;

  StringArray get_shader_source_files(const String &name);
};

//-----------------------------------------------------------------------------
//
// Material Loader
//
//-----------------------------------------------------------------------------

typedef uptr<Material> (*MaterialCreateFunc)(ResourceService &);

struct MaterialCreator {
  String             name;
  MaterialCreateFunc create;

  MaterialCreator(const String &material_name, const MaterialCreateFunc material_creator)
    : name(material_name)
    , create(material_creator)
  {}
};

class MaterialLoader : public Loader {
public:
  MaterialLoader();
  ~MaterialLoader();

  ResourcePtr create_resource(ResourceService &rs, const String &name) override;

  void reload_resource(ResourceService &rs, Resource &resource) override;

  std::vector<MaterialCreator> material_creators;

  static String get_material_filename(const String &name);
};

//-----------------------------------------------------------------------------
//
// Mesh Loader
//
//-----------------------------------------------------------------------------

class MeshLoader : public Loader {
public:
  ResourcePtr create_resource(ResourceService &rs, const String &name) override;

  void reload_resource(ResourceService &rs, Resource &resource) override;

private:
  static void load_mesh_from_model(ResourceService &rs, const RawMesh &model, Mesh &mesh);
};


//-----------------------------------------------------------------------------
//
// Bitmap Font Loader
//
//-----------------------------------------------------------------------------

class BitmapFontLoader : public Loader {
public:
  ResourcePtr create_resource(ResourceService &rs, const String &name) override;

  void reload_resource(ResourceService &rs, Resource &resource) override;
};

//-----------------------------------------------------------------------------
//
// Sound Loader
//
//-----------------------------------------------------------------------------

class SoundLoader : public Loader {
public:
  ResourcePtr create_resource(ResourceService &rs, const String &name) override;

  void reload_resource(ResourceService &rs, Resource &resource) override;

private:
  String sound_filename(const String &name);
};

//-----------------------------------------------------------------------------
//
// Music Loader
//
//-----------------------------------------------------------------------------

class MusicLoader : public Loader {
public:
  ResourcePtr create_resource(ResourceService &rs, const String &name);

  void reload_resource(ResourceService &rs, Resource &resource);

private:
  static String music_file(const String &name);
};

}
