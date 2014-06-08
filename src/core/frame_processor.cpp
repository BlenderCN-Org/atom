#include "core/frame_processor.h"

#include <chrono>
#include <thread>
#include "core/system/core.h"
#include "core/input/input_service.h"
#include "core/video/video_service.h"
#include "core/video/draw_service.h"
#include "core/audio/audio_service.h"
#include "core/system/resource_service.h"
#include "core/frame.h"
#include "core/log.h"
#include "core/system/config.h"
#include "core/math/transformations.h"
#include "core/math/transformation_stack.h"
#include "core/video/bitmap_font.h"
#include "core/video/uniforms.h"
#include "core/system/sdl.h"

//using std::chrono::high_resolution_clock;
//using std::chrono::duration_cast;
//using std::this_thread::sleep_for;
using std::placeholders::_1;
using std::bind;

namespace atom {

FrameProcessor::FrameProcessor(Core &core)
  : my_core(core)
  , my_post_frame_callback(nullptr)
{
  my_font = core.resource_service().get_bitmap_font_resource("font");
}

FrameProcessor::~FrameProcessor()
{
}

void FrameProcessor::run(const FramePtr &frame)
{
  assert(frame != nullptr);

  my_current_frame = frame;

  while (my_current_frame != nullptr) {
//    my_current_frame->performance_counters().clear();
//    auto last = std::chrono::high_resolution_clock::now();
    std::chrono::microseconds frame_time(1000000 / FPS);

//    my_core.input_service().set_event_func(bind(&Frame::process_event, this, _1));

  /// @todo frame skipping on slow graphics
    PerformanceCounters &counters = my_current_frame->performance_counters();
    my_core.input_service().poll();

    // emergency abort by Ctrl+Shift+Q
    InputService &ip = my_core.input_service();
    if (ip.is_key_pressed(Key::KEY_Q) &&
        (ip.is_key_pressed(Key::KEY_LSHIFT) || ip.is_key_pressed(Key::KEY_RSHIFT)) &&
        (ip.is_key_pressed(Key::KEY_LCTRL) || ip.is_key_pressed(Key::KEY_RCTRL))) {
      log::warning("Emergency Abort");
      break;
    }

    counters.start("Input processing");
    my_current_frame->input();
    counters.stop("Input processing");

    counters.start("Frame update");
    my_current_frame->update();
    counters.stop("Frame update");

    counters.start("Frame rendering");
    my_current_frame->draw();
    counters.stop("Frame rendering");

//    log::info("Drawing counter");
    // render counters
    draw_counters(counters.to_string());
    // clear counters and start a new measurement
    counters.clear();

//    my_counters.start("Poll");
//    my_core.audio_service().update();
//    my_counters.stop("Poll");

    counters.start("Resource system");
    my_core.resource_service().poll();
    counters.stop("Resource system");

    if (my_post_frame_callback != nullptr) {
      my_post_frame_callback(nullptr);
    } else {
      log::warning("Missing frame buffer update callback");
    }

//    auto now = high_resolution_clock::now();
//    auto duration = duration_cast<std::chrono::microseconds>(now - last);
//    if (duration < frame_time) {
//      sleep_for(frame_time - duration);
//    }

//    last = now;

//    if (my_post_frame_callback) {
//      if (my_post_frame_callback() == true)
//        break;
//    }

    if (!my_current_frame->is_running()) {
      my_current_frame = my_current_frame->next_frame(my_current_frame);
      // reset running state
      if (my_current_frame != nullptr)
        my_current_frame->set_running_state(true);
    }

    my_core.resource_service().garbage_collect();
  }
}

void FrameProcessor::set_post_frame_callback(PostFrameCallback callback)
{
  my_post_frame_callback = callback;
}

void FrameProcessor::draw_counters(
  const String &text)
{
  Config &config = Config::instance();
  Uniforms &u = my_core.video_service().get_uniforms();

  u.transformations.model = Mat4f::identity();
  u.transformations.view = Mat4f::identity();
  u.transformations.projection =
    Mat4f::orthographic(0.0f, 100.0f, 0.0f, 100.0f / config.get_aspect_ratio(), 0.0f, 1000.0f);

  // draw performance counters
// chyba implementacia
//  my_core.draw_service().draw_text(my_font->bitmap_font(),
//    Vec2f(1, 100 / config.get_aspect_ratio() - 3), 2, text.c_str(), Vec4f(1, 1, 1, 1));
  // draw big text
//  my_core.draw_service().draw_text(my_font->bitmap_font(), Vec2f(1, 100 / config.get_aspect_ratio() - 15),
//    14, text.c_str(), Vec4f(0, 0.2, 0.3, 1));
}

}
