#include "core/frame.h"

#include "core/log.h"
#include "core/video/video_service.h"
#include "core/audio/audio_service.h"
#include "core/input/input_service.h"
#include "core/system/resource_service.h"

namespace atom {

Frame::Frame(Core &core)
  : my_core(core)
  , my_is_running(true)
{
}

Frame::~Frame()
{
}

FramePtr Frame::next_frame(
  const FramePtr &)
{
  return nullptr;
}

void Frame::process_input_event(const Event &event)
{
  // empty for subclass reimplementation
}

void Frame::exit_frame()
{
  my_is_running = false;
}

}
