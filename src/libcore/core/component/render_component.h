#pragma once

#include "../video/material.h"
#include "component.h"

namespace atom {

class RenderComponent : public NullComponent {
  Slot<MaterialComponent> my_material;
  Slot<MeshComponent>     my_mesh;
  bool                    my_is_enabled;

  uptr<Component> clone() const override;

  void activate() override;

  void deactivate() override;

public:
  RenderComponent();

  MaterialResourcePtr material() const;

  MeshResourcePtr mesh() const;

  bool is_enabled() const
  {
    return my_is_enabled;
  }

  void set_enabled(bool enabled)
  {
    my_is_enabled = enabled;
  }
};

MAP_COMPONENT_TYPE(RenderComponent, RENDER)

}
