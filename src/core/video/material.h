#pragma once

#include <memory>
#include "core/corefwd.h"
#include "core/noncopyable.h"
#include "core/math/vec.h"
#include "core/meta.h"
#include "core/video/video_service.h"
#include "core/video/texture_sampler.h"
#include "core/video/technique.h"
#include "core/video/shader.h"

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
// Flat Material
//
//-----------------------------------------------------------------------------

class PixmapMaterial : public Material {
public:
  static uptr<Material> create(ResourceService &rs);

  PixmapMaterial(VideoService &vs, const TechniqueResourcePtr &pixmap_shader);

  ~PixmapMaterial();

  void draw_mesh(const RenderContext &context, const Mesh &mesh) override;

  TechniqueResourcePtr  my_shader;
  TextureResourcePtr my_texture;
  TextureSampler     my_sampler;
  Vec3f              my_color;

  META_DECLARE_CLASS;
private:
  Technique my_program;
};

}
