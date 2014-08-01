#include "uniforms.h"

namespace atom {

META_DEFINE_FIELDS(Uniforms) {
  FIELD(Uniforms, color, "color"),
  FIELD(Uniforms, ambient_color, "ambient"),
  FIELD(Uniforms, sun_dir, "sun_dir"),
  FIELD(Uniforms, model, "model"),
  FIELD(Uniforms, mvp, "mvp"),
  FIELD(Uniforms, bones, "bones[0]")
//  FIELD(Uniforms, color, "color"),
//  FIELD(Uniforms, color, "color"),
};

META_DEFINE_ROOT_CLASS(Uniforms, "Uniforms");

Uniforms::Uniforms()
  : color(0.5, 0.5, 0.5)
  , ambient_color(1, 1, 1)
{
  META_INIT();

  bones.resize(256, Mat4f::identity());
}

}
