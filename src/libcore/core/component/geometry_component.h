#pragma once

#include "component.h"

namespace atom {

class GeometryComponent : public NullComponent {
  
  void init() override;
  void terminate() override;
  
  uptr<Component> clone() const override;
  
public:
  GeometryComponent();
};

}
