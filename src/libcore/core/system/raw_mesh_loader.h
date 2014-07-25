#pragma once

#include "loaders.h"

namespace atom {

//-----------------------------------------------------------------------------
//
// RawMesh Loader
//
//-----------------------------------------------------------------------------

bool load_model(const String &filename, RawMesh &model);

uptr<RawMesh> load_model(const String &filename);

class RawMeshLoader : public Loader {
public:
  ResourcePtr create_resource(ResourceService &rs, const String &name) override;

  void reload_resource(ResourceService &rs, Resource &resource) override;

private:
  static String get_model_filename(const String &name);

};

}
