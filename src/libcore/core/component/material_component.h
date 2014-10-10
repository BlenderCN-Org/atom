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
  MaterialComponent();
  
  void set_material_name(const String &name);

  MaterialResourcePtr material() const;
  
  META_SUB_CLASS(NullComponent);
};

MAP_COMPONENT_TYPE(MaterialComponent, MATERIAL)

}
