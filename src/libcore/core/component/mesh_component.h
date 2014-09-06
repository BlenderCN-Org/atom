#pragma once

#include "component.h"

namespace atom {

class MeshComponent : public Component {
  Slot<ModelComponent> my_model;
  MeshResourcePtr      my_mesh;

  uptr<Component> clone() const override;

  void activate() override;

  void deactivate() override;

public:
  MeshComponent();
  ~MeshComponent();

  MeshResourcePtr mesh() const;
};

MAP_COMPONENT_TYPE(MeshComponent, MESH)

}
