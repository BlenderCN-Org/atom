#include "processor.h"
#include "core.h"
#include "world.h"

namespace atom {

Processor::Processor(World &world)
  : my_world(world)
{
  my_world.register_processor(this);
}

Processor::~Processor()
{
  // empty
  my_world.unregister_processor(this);
}

Core& Processor::core() const
{
  return my_world.core();
}

World& Processor::world() const
{
  return my_world;
}

}
