#pragma once

#include "core/corefwd.h"

namespace atom {

void render_scene(
  VideoService &vs,
  DrawService &dp,
  const MeshTree &mesh_tree,
  const Camera &camera);

}
