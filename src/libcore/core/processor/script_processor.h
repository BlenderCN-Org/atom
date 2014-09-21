#pragma once

#include <vector>
#include "../corefwd.h"
#include "processor.h"

namespace atom {

class ScriptProcessor : public NullProcessor {
  bool                           my_is_started;
  std::vector<ScriptComponent *> my_scripts;

public:
  explicit ScriptProcessor(World &world);

  void register_script(ScriptComponent *component);
  void unregister_script(ScriptComponent *component);

  void activate() override;

  void poll() override;
};

}
