#pragma once

#include "core/corefwd.h"
#include "core/noncopyable.h"
#include "core/frame.h"

namespace atom {

typedef void (*PostFrameCallback)(void *);

class FrameProcessor : private NonCopyable {
  Core               &my_core;
  FramePtr              my_current_frame;
  BitmapFontResourcePtr my_font;
  PostFrameCallback     my_post_frame_callback;

public:
  FrameProcessor(Core &core);
  ~FrameProcessor();

  void run(const FramePtr &frame);

  void set_post_frame_callback(PostFrameCallback callback);

private:
  void draw_counters(const String &text);
};

}
