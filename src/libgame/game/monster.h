#pragma once

#include <core/component/script_component.h>

namespace atom {

class MonsterScript : public ScriptComponent {
  Slot<SkeletonComponent> my_skeleton;

public:
  MonsterScript();

  void update() override;

private:
  uptr<Component> clone() const override;
};

}
