#pragma once

#include "component.h"
#include "core/video/material.h"

namespace atom {

class MaterialComponent : public Component {
  String my_material_name;
  MaterialResourcePtr my_material;

  void activate() override;

  void deactivate() override;

  uptr<Component> clone() const override;

public:
  explicit MaterialComponent(const String &name);

  MaterialResourcePtr material() const;
};

MAP_COMPONENT_TYPE(MaterialComponent, MATERIAL)

}
