#pragma once

#include <memory>
#include "corefwd.h"
#include "noncopyable.h"
#include "performance_counters.h"

namespace atom {

typedef sptr<Frame> FramePtr;

class Frame : private NonCopyable {
public:
  Frame(Core &core);

  virtual ~Frame();

  virtual FramePtr next_frame(const FramePtr &current_frame);

  virtual void process_input_event(const Event &event);

  virtual void input() = 0;

  virtual void update() = 0;

  virtual void draw() = 0;

  void set_running_state(bool running)
  {
    my_is_running = running;
  }

  bool is_running()
  {
    return my_is_running;
  }

  PerformanceCounters& performance_counters()
  {
    return my_counters;
  }

  void exit_frame();

  Core& core()
  {
    return my_core;
  }

private:
  Core               &my_core;
  PerformanceCounters my_counters;
  bool                my_is_running;
};

}
