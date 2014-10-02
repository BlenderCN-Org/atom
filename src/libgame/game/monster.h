#pragma once

#include <core/component/script_component.h>

namespace atom {

class MonsterScript : public ScriptComponent {
  Slot<SkeletonComponent> my_skeleton;
  
  uptr<Component> clone() const override;
  
  void on_update() override;
  
public:
  MonsterScript();
};

}
