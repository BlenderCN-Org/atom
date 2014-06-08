#pragma once

#include "core/noncopyable.h"

namespace atom {

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

private:
};

}
