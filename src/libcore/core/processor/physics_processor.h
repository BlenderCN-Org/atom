#pragma once

#include <vector>
#include "../corefwd.h"
#include "../math/mat.h"
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
