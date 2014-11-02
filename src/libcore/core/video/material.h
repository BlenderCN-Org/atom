#pragma once

#include <memory>
#include "../foundation.h"
#include "video_service.h"
#include "texture_sampler.h"
#include "technique.h"
#include "shader.h"
#include "../system/resources.h"

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


//
// Lines Material - vertices, color
//

class LinesMaterial : public Material {
  TechniqueResourcePtr my_shader;
  Vec3f                color;
  
public:
  static uptr<Material> create(ResourceService &rs);

  LinesMaterial(const TechniqueResourcePtr &shader);

  ~LinesMaterial();

  void draw_mesh(const RenderContext &context, const Mesh &mesh) override;
  
  META_SUB_CLASS(Material);
};


//
// Flat Material - vertices, indices, color
//

class FlatMaterial : public Material {
  TechniqueResourcePtr my_shader;
  Vec3f                my_color;
  
public:
  static uptr<Material> create(ResourceService &rs);
  
  FlatMaterial(const TechniqueResourcePtr &shader);
  
  ~FlatMaterial();
  
  void draw_mesh(const RenderContext &context, const Mesh &mesh) override;
  
  META_SUB_CLASS(Material);
};


//
// WireframeMaterial - vertices, indices, color
//

class WireframeMaterial : public Material {
  TechniqueResourcePtr my_shader;
  Vec3f my_color;
  
public:
  static uptr<Material> create(ResourceService &rs);
  
  WireframeMaterial(const TechniqueResourcePtr &shader);
  
  ~WireframeMaterial();
  
  void draw_mesh(const RenderContext &context, const Mesh &mesh) override;
  
  META_SUB_CLASS(Material);
};


//
// PhongMaterial - vertices, normals, indices
//

class PhongMaterial : public Material {
  TechniqueResourcePtr my_program;
  
public:
  static uptr<Material> create(ResourceService &rs);

  PhongMaterial(const TechniqueResourcePtr &shader);

  ~PhongMaterial();

  void draw_mesh(const RenderContext &context, const Mesh &mesh) override;
  
  META_SUB_CLASS(Material);

  TechniqueResourcePtr my_shader;
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
