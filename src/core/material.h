#pragma once

#include <memory>
#include "foundation.h"
#include "video_service.h"
#include "texture_sampler.h"
#include "technique.h"
#include "shader.h"
#include "resources.h"

namespace atom {

// TODO:
//   PhongSkin vertices, normals, bones, indices

class Material : private NonCopyable {
  DrawFace my_face;

public:
  Material()
    : my_face(DrawFace::FRONT)
  {
    META_INIT();
  }

  virtual ~Material();

  virtual void draw_mesh(const RenderContext &context, const Mesh &mesh) = 0;

  DrawFace face() const
  {
    return my_face;
  }


  META_ROOT_CLASS;
};

typedef sptr<Material> MaterialPtr;

class SimpleMaterial : public Material {
  u32                  my_flags;
  DrawFace             my_draw_face;
  DrawType             my_draw_type;
  FillMode             my_fill_mode;
  bool                 my_depth_test;
  TechniqueResourcePtr my_technique;

public:
  enum DrawFlags {
    VERTEX = 1,
    NORMAL = 2,
    COLOR = 4,
    INDEX = 32
  };

  static uptr<Material> create(ResourceService &rs);

  explicit SimpleMaterial(u32 draw_flags);
  ~SimpleMaterial();

  void draw_mesh(const RenderContext &context, const Mesh &mesh) override;

  META_SUB_CLASS(Material);
};

//
// DebugMaterial (wireframe/lines)
//

class DebugMaterial : public SimpleMaterial {
public:
  static uptr<Material> create(ResourceService &rs);

  DebugMaterial();
};


//
// Flat Material - vertices, indices, color
//

class FlatMaterial : public SimpleMaterial {
  Vec3f my_color;

public:
  static uptr<Material> create(ResourceService &rs);

  FlatMaterial();

  ~FlatMaterial();

  void draw_mesh(const RenderContext &context, const Mesh &mesh) override;

  META_SUB_CLASS(SimpleMaterial);
};


//
// Road Material - vertices, indices, color
//

class RoadMaterial : public SimpleMaterial {
  Vec3f              my_color;
  TextureResourcePtr my_texture;

public:
  static uptr<Material> create(ResourceService &rs);

  RoadMaterial();

  ~RoadMaterial();

  void draw_mesh(const RenderContext &context, const Mesh &mesh) override;

  META_SUB_CLASS(SimpleMaterial);
};


//
// PhongMaterial - vertices, normals, indices
//

class PhongMaterial : public SimpleMaterial {
  TechniqueResourcePtr my_program;

public:
  static uptr<Material> create(ResourceService &rs);

  PhongMaterial(const TechniqueResourcePtr &shader);

  ~PhongMaterial();

  void draw_mesh(const RenderContext &context, const Mesh &mesh) override;

  META_SUB_CLASS(SimpleMaterial);

  Vec3f                my_color;
};


//
// FlatSkinMaterial - flat shading + animated by bones
//                  - vertices, weight, bones, indices
//

class FlatSkinMaterial : public Material {
  TechniqueResourcePtr my_program;

public:
  static uptr<Material> create(ResourceService &rs);

  FlatSkinMaterial(const TechniqueResourcePtr &shader);

  ~FlatSkinMaterial();

  void draw_mesh(const RenderContext &context, const Mesh &mesh) override;

  TechniqueResourcePtr my_shader;
  Vec3f                my_color;

  META_SUB_CLASS(Material);
};

}
