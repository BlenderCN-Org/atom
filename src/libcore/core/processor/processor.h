#pragma once

#include "../noncopyable.h"

namespace atom {

/// @todo replace start method with init/terminate & activate/deactivate
class Processor : private NonCopyable {
public:
  virtual ~Processor();

  /**
   * Update/process processor state.
   */
  virtual void poll() = 0;

  /**
   * Start/wake up all registered components.
   */
  virtual void start() = 0;

  virtual void init()
  {
    // empty, reimplemented in subclass, don't need to be called
  }

  virtual void terminate()
  {
    // empty, reimplemented in subclass, don't need to be called
  }

  virtual void activate()
  {
    // empty, reimplemented in subclass, don't need to be called
  }

  virtual void deactivate()
  {
    // empty, reimplemented in subclass, don't need to be called
  }

private:
};

}
