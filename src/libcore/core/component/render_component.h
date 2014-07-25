#pragma once

#include "../video/material.h"
#include "component.h"

namespace atom {

class RenderComponent : public Component {
  Slot<MaterialComponent> my_material;
  Slot<MeshComponent> my_mesh;

  uptr<Component> clone() const override;

  void activate() override;

  void deactivate() override;

public:
  RenderComponent();

  MaterialResourcePtr material() const;

  MeshResourcePtr mesh() const;
};

MAP_COMPONENT_TYPE(RenderComponent, RENDER)

}
