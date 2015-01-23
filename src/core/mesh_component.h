#pragma once

#include "component.h"

namespace atom {

enum class MeshComponentMode : u32 {
  AUTO,
  MANUAL
};

TYPE_OF(MeshComponentMode, U32)
MAP_COMPONENT_TYPE(MeshComponent, MESH)

class MeshComponent : public NullComponent {
  MeshComponentMode    my_mode;
  Slot<ModelComponent> my_model;
  MeshResourcePtr      my_mesh;

  void activate() override;

  void deactivate() override;

public:
  MeshComponent();
  ~MeshComponent();

  MeshResourcePtr mesh() const;

  void set_mesh(MeshResourcePtr mesh);

  void set_mode(MeshComponentMode mode);

  META_SUB_CLASS(NullComponent);
};

}
