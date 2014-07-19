#pragma once

#include "component.h"
#include "core/video/material.h"

namespace atom {

class RenderComponent : public Component {
  MeshComponent &my_mesh_component;
  MaterialResourcePtr my_material;

public:
  explicit RenderComponent(MeshComponent &mesh,
    const MaterialResourcePtr &material);

  void attach() override;
  void detach() override;

  uptr<Component> clone() const override;
  
  const MaterialResourcePtr& material() const;
  
  MeshResourcePtr mesh() const;
};

MAP_COMPONENT_TYPE(RenderComponent, RENDER)

}
