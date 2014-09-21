#pragma once

#include "../foundation.h"

namespace atom {

/// @todo replace start method with init/terminate & activate/deactivate
class Processor : NonCopyable {
  World &my_world;

protected:
  explicit Processor(World &world);
  virtual ~Processor();

  Core& core() const;

  World& world() const;

public:
  virtual void init() = 0;

  virtual void terminate() = 0;

  virtual void activate() = 0;

  virtual void deactivate() = 0;

  /**
   * Update/process processor state.
   */
  virtual void poll() = 0;
};

class NullProcessor : public Processor {
protected:
  NullProcessor(World &world)
    : Processor(world)
  {
    // empty
  }

public:
  void init() override
  {
    // empty
  }

  void terminate() override
  {
    // empty
  }

  void activate() override
  {
    // empty
  }

  void deactivate() override
  {
    // empty
  }

  void poll() override
  {
    // empty
  }
};

}
