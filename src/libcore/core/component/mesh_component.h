#pragma once

#include "component.h"

namespace atom {

enum class MeshComponentMode {
  AUTO,
  MANUAL
};

class MeshComponent : public Component {
  MeshComponentMode    my_mode;
  Slot<ModelComponent> my_model;
  MeshResourcePtr      my_mesh;

  uptr<Component> clone() const override;

  void activate() override;

  void deactivate() override;

public:
  explicit MeshComponent(MeshComponentMode mode = MeshComponentMode::AUTO);
  ~MeshComponent();

  MeshResourcePtr mesh() const;

  void set_mesh(MeshResourcePtr mesh);
};

MAP_COMPONENT_TYPE(MeshComponent, MESH)

}
