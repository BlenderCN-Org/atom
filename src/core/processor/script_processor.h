#pragma once

#include <vector>
#include "core/corefwd.h"
#include "core/processor/processor.h"

namespace atom {

class ScriptProcessor : public Processor {
public:
  void register_script(SensorComponent *component);
  void unregister_script(SensorComponent *component);

  void poll() override;

  void start() override;

private:
};

}
