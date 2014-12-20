#include "uniforms.h"

namespace atom {

META_CLASS(Uniforms,
  FIELD(color, "color"),
  FIELD(ambient_color, "ambient"),
  FIELD(sun_dir, "sun_dir"),
  FIELD(model, "model"),
  FIELD(mvp, "mvp"),
  FIELD(bones, "bones[0]")
)

Uniforms::Uniforms()
  : color(0.5, 0.5, 0.5)
  , ambient_color(1, 1, 1)
{
  META_INIT();

  bones.resize(256, Mat4f());
}

}
