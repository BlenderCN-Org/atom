#pragma once

#include "../video/material.h"
#include "component.h"

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