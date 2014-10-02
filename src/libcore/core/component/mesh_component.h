#pragma once

#include "component.h"

namespace atom {

enum class MeshComponentMode : u32 {
  AUTO,
  MANUAL
};

class MeshComponent : public NullComponent {
  MeshComponentMode    my_mode;
  Slot<ModelComponent> my_model;
  MeshResourcePtr      my_mesh;

  uptr<Component> clone() const override;

  void activate() override;

  void deactivate() override;

public:
  META_DECLARE_CLASS;
  
  explicit MeshComponent(MeshComponentMode mode = MeshComponentMode::AUTO);
  ~MeshComponent();

  MeshResourcePtr mesh() const;

  void set_mesh(MeshResourcePtr mesh);
};

MAP_TYPE(MeshComponentMode, U32)
MAP_COMPONENT_TYPE(MeshComponent, MESH)

}
