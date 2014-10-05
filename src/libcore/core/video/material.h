#pragma once

#include <memory>
#include "../foundation.h"
#include "video_service.h"
#include "texture_sampler.h"
#include "technique.h"
#include "shader.h"

namespace atom {

//FlatSkin vertices, weight, bones, indices
//PhongSkin vertices, normals, bones, indices

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


//
// Lines Material - vertices, color
//

class LinesMaterial : public Material {
public:
  META_DECLARE_CLASS;
  
  static uptr<Material> create(ResourceService &rs);

  LinesMaterial(const TechniqueResourcePtr &shader);

  ~LinesMaterial();

  void draw_mesh(const RenderContext &context, const Mesh &mesh) override;

  TechniqueResourcePtr my_shader;
  Vec3f color;
};


//
// Flat Material - vertices, indices, color
//

class FlatMaterial : public Material {
public:
  static uptr<Material> create(ResourceService &rs);
  
  FlatMaterial(const TechniqueResourcePtr &shader);
  
  ~FlatMaterial();
  
  void draw_mesh(const RenderContext &context, const Mesh &mesh) override;
  
  TechniqueResourcePtr my_shader;
  Vec3f color;
  
  META_DECLARE_CLASS;
};


//
// WireframeMaterial - vertices, indices, color
//

class WireframeMaterial : public Material {
public:
  static uptr<Material> create(ResourceService &rs);
  
  WireframeMaterial(const TechniqueResourcePtr &shader);
  
  ~WireframeMaterial();
  
  void draw_mesh(const RenderContext &context, const Mesh &mesh) override;
  
  TechniqueResourcePtr my_shader;
  Vec3f color;
  
  META_DECLARE_CLASS;
};


//
// PhongMaterial - vertices, normals, indices
//

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


//
// FlatSkinMaterial - flat shading + animated by bones
//                  - vertices, weight, bones, indices
//

class FlatSkinMaterial : public Material {
public:
  static uptr<Material> create(ResourceService &rs);

  FlatSkinMaterial(const TechniqueResourcePtr &shader);

  ~FlatSkinMaterial();

  void draw_mesh(const RenderContext &context, const Mesh &mesh) override;

  TechniqueResourcePtr my_shader;
  Vec3f                my_color;

  META_DECLARE_CLASS;
private:
  TechniqueResourcePtr my_program;
};

}
