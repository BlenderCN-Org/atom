#pragma once

#include "core/corefwd.h"
#include "core/noncopyable.h"
#include "core/frame.h"

namespace atom {

//typedef std::function<bool ()> PostFrameCallback;

typedef void (*PostFrameCallback)(void *);

class FrameProcessor : private NonCopyable {
public:
  FrameProcessor(Core &core);
  ~FrameProcessor();

  void run(const FramePtr &frame);

  void set_post_frame_callback(PostFrameCallback callback);

//  void set_post_frame_callback(PostFrameCallback callback)
//  { my_post_frame_callback = callback; }

private:
  void draw_counters(
    const String &text);

private:
  Core               &my_core;
  FramePtr              my_current_frame;
  BitmapFontResourcePtr my_font;
  PostFrameCallback     my_post_frame_callback;
};

}
