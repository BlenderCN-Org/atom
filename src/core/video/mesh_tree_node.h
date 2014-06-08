#pragma once

#include <vector>
#include "core/corefwd.h"
#include "core/noncopyable.h"
#include "core/video/material.h"
#include "core/math/mat.h"
#include "core/video/mesh.h"

namespace atom {

class MeshTreeNode : private NonCopyable {
public:
  MeshTreeNode();
  ~MeshTreeNode();

  Mat4f               transformations;
  sptr<Mesh>          mesh;
  MaterialResourcePtr material;
  bool                visible;
  MeshResourcePtr     mesh_resource;

  std::vector<sptr<MeshTreeNode>> children;
};

}
