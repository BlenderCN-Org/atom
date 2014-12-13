#pragma once

#include "../foundation.h"
#include "video_buffer.h"

namespace atom {

class Mesh : private NonCopyable {
public:     // public methods
  uptr<VideoBuffer> vertex;       ///< mesh vertices (Vec3f)
  uptr<VideoBuffer> normal;       ///< mesh normals
  uptr<VideoBuffer> color;       ///< mesh vertex colors
  uptr<VideoBuffer> uv;
  uptr<VideoBuffer> surface;      ///< triangle indices (u32)
  uptr<VideoBuffer> bone_weight;  ///< per vertex bone weights (Vec4f)
  uptr<VideoBuffer> bone_index;   ///< per vertex bone indices (Vec4u8)
};

}
