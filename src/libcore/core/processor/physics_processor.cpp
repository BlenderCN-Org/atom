#include "physics_processor.h"

namespace atom {

PhysicsProcessor::PhysicsProcessor(VideoService &vs, ResourceService &rs)
  : my_is_running(false)
{
//  set_enable_debug(true);
}

PhysicsProcessor::~PhysicsProcessor()
{

}

void PhysicsProcessor::poll()
{
  if (!my_is_running) {
    return;
  }
}

void PhysicsProcessor::start()
{
  assert(my_is_running == false);
  my_is_running = true;
}

}
