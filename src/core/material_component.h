#pragma once

#include "material.h"
#include "component.h"

namespace atom {

class MaterialComponent : public NullComponent {
  MaterialResourcePtr my_material;

  void activate() override;

  void deactivate() override;

public:
  MaterialComponent();

  void set_material(const MaterialResourcePtr material);

  MaterialResourcePtr material() const;

  META_SUB_CLASS(NullComponent);
};

MAP_COMPONENT_TYPE(MaterialComponent, MATERIAL)

}
