#pragma once

#include "../video/material.h"
#include "component.h"

namespace atom {

class MaterialComponent : public NullComponent {
  String my_material_name;
  MaterialResourcePtr my_material;

  void activate() override;

  void deactivate() override;

  uptr<Component> clone() const override;

public:
  META_DECLARE_CLASS;
  
  MaterialComponent();
  
  void set_material_name(const String &name);

  MaterialResourcePtr material() const;
};

MAP_COMPONENT_TYPE(MaterialComponent, MATERIAL)

}
