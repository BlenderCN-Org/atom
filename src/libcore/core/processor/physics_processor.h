#pragma once

#include <vector>
#include "../foundation.h"
#include "processor.h"

namespace atom {

class Box2DDebugDraw;

class PhysicsProcessor : public Processor {
  bool my_is_running;

public:
  PhysicsProcessor(VideoService &vs, ResourceService &rs);
  ~PhysicsProcessor();

  void poll() override;

  void start() override;
};

}
