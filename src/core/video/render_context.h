#pragma once

#include "core/corefwd.h"

namespace atom {

struct RenderContext {
  Uniforms     &uniforms;
  VideoService &video_processor;
  DrawService  &draw_processor;
};

}
