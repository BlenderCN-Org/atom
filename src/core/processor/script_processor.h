#pragma once

#include <vector>
#include "core/corefwd.h"
#include "core/processor/processor.h"

namespace atom {

class ScriptProcessor : public Processor {
  bool                           my_is_started;
  std::vector<ScriptComponent *> my_scripts;

public:
  ScriptProcessor();

  void register_script(ScriptComponent *component);
  void unregister_script(ScriptComponent *component);

  void poll() override;

  void start() override;
};

}
