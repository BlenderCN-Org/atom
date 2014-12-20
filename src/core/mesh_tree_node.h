#pragma once

#include <vector>
#include "corefwd.h"
#include "noncopyable.h"
#include "math.h"
#include "material.h"
#include "mesh.h"



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
