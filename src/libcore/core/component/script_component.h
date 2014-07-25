#pragma once

#include "component.h"

namespace atom {

class ScriptComponent : public Component {

  void activate() override;

  void deactivate() override;

public:
  ScriptComponent();

  virtual void update() = 0;
};

MAP_COMPONENT_TYPE(ScriptComponent, SCRIPT)

}
