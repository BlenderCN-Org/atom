#pragma once

#include <memory>
#include "../corefwd.h"
#include "../noncopyable.h"
#include "../math/vec.h"
#include "../meta.h"
#include "video_service.h"
#include "texture_sampler.h"
#include "technique.h"
#include "shader.h"

namespace atom {

class Material : private NonCopyable {
public:
  Material()
    : face(DrawFace::FRONT)
  {
    META_INIT();
  }

  virtual ~Material();

  virtual void draw_mesh(const RenderContext &context, const Mesh &mesh) = 0;

  DrawFace face;

  META_DECLARE_CLASS_PTR; // each instance contains pointer to the MetaClass
  META_DECLARE_CLASS;     // static instance of MetaClass for Material
};

typedef sptr<Material> MaterialPtr;

//-----------------------------------------------------------------------------
//
// Flat Material
//
//-----------------------------------------------------------------------------

class FlatMaterial : public Material {
public:
  static uptr<Material> create(ResourceService &rs);

  FlatMaterial(const TechniqueResourcePtr &phong_shader);

  ~FlatMaterial();

  void draw_mesh(const RenderContext &context, const Mesh &mesh) override;

  TechniqueResourcePtr shader;

  Vec3f color;

  META_DECLARE_CLASS;
};

//-----------------------------------------------------------------------------
//
// Phong Material
//
//-----------------------------------------------------------------------------

class PhongMaterial : public Material {
public:
  static uptr<Material> create(ResourceService &rs);

  PhongMaterial(const TechniqueResourcePtr &shader);

  ~PhongMaterial();

  void draw_mesh(const RenderContext &context, const Mesh &mesh) override;

  TechniqueResourcePtr my_shader;
  Vec3f                my_color;

  META_DECLARE_CLASS;
private:
  TechniqueResourcePtr my_program;
};

}
