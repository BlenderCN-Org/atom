#pragma once

#include <core/script_component.h>

namespace atom {

class MonsterScript : public ScriptComponent {
  Slot<SkeletonComponent> my_skeleton;

  void on_update() override;

public:
  MonsterScript();
};

}
