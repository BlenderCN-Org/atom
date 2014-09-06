#include "material.h"
#include "../system/resources.h"
#include "../system/resource_service.h"
#include "uniforms.h"
#include "render_context.h"
#include "video_service.h"
#include "mesh.h"

namespace atom {

META_DEFINE_FIELDS(Material) {
  FIELD(Material, face, "face")
};

META_DEFINE_ROOT_CLASS(Material, "Material");

Material::~Material()
{
}

//-----------------------------------------------------------------------------
//
// Flat Material
//
//-----------------------------------------------------------------------------

META_DEFINE_FIELDS(FlatMaterial) {
  FIELD(FlatMaterial, my_shader, "shader"),
  FIELD(FlatMaterial, color, "color")
};

META_DEFINE_CLASS(FlatMaterial, Material, "FlatMaterial");

uptr<Material> FlatMaterial::create(ResourceService &rs)
{
  return uptr<Material>(new FlatMaterial(rs.get_technique("flat")));
}

FlatMaterial::FlatMaterial(const TechniqueResourcePtr &shader)
  : my_shader(shader)
{
  META_INIT();
}

FlatMaterial::~FlatMaterial()
{
}

void FlatMaterial::draw_mesh(const RenderContext &context, const Mesh &mesh)
{
  assert(my_shader != nullptr);

  if (mesh.vertex == nullptr || mesh.surface == nullptr) {
    log::warning("%s: mesh missing vertex or surface data", ATOM_FUNC_NAME);
    return;
  }

  context.uniforms.color = color;

  DrawCommand command;
  command.draw = DrawType::LINES;
  command.attributes[0] = mesh.vertex.get();
  command.types[0] = Type::VEC3F;
  command.indices = mesh.surface.get();
  command.program = &my_shader->program();
  context.video_processor.draw(command);
}


//-----------------------------------------------------------------------------
//
// Phong Material
//
//-----------------------------------------------------------------------------

META_DEFINE_FIELDS(PhongMaterial) {
  FIELD(PhongMaterial, my_shader, "shader"),
  FIELD(PhongMaterial, my_color, "color"),
};

META_DEFINE_CLASS(PhongMaterial, Material, "PhongMaterial");


uptr<Material> PhongMaterial::create(ResourceService &rs)
{
  return uptr<Material>(new PhongMaterial(rs.get_technique("phong")));
}

PhongMaterial::PhongMaterial(const TechniqueResourcePtr &shader)
  : my_program(shader)
{
  META_INIT();
}

PhongMaterial::~PhongMaterial()
{
}

void PhongMaterial::draw_mesh(const RenderContext &context, const Mesh &mesh)
{
  assert(my_shader != nullptr);

  if (mesh.vertex == nullptr || mesh.surface == nullptr) {
    log::warning("%s: mesh missing vertex or surface data", ATOM_FUNC_NAME);
  }

  VideoService &vs = context.video_processor;
  vs.enable_depth_test();
  context.uniforms.color = Vec3f(0.7, 0.7, 0.7);
  context.uniforms.sun_dir = Vec3f(0, 0, -1);
  context.uniforms.ambient_color = Vec3f(0.13, 0.13, 0.13);

  DrawCommand command;
  command.draw = DrawType::TRIANGLES;
  command.attributes[0] = mesh.vertex.get();
  command.types[0] = Type::VEC3F;
  command.indices = mesh.surface.get();
  command.program = &my_shader->program();
  context.video_processor.draw(command);
}


//-----------------------------------------------------------------------------
//
// Skin Material
//
//-----------------------------------------------------------------------------

META_DEFINE_FIELDS(SkinMaterial) {
  FIELD(SkinMaterial, my_shader, "shader"),
  FIELD(SkinMaterial, my_color, "color"),
};

META_DEFINE_CLASS(SkinMaterial, Material, "SkinMaterial");


uptr<Material> SkinMaterial::create(ResourceService &rs)
{
  return uptr<Material>(new SkinMaterial(rs.get_technique("skin")));
}

SkinMaterial::SkinMaterial(const TechniqueResourcePtr &shader)
  : my_program(shader)
{
  META_INIT();
}

SkinMaterial::~SkinMaterial()
{
}

void SkinMaterial::draw_mesh(const RenderContext &context, const Mesh &mesh)
{
  assert(my_shader != nullptr);

  if (mesh.vertex == nullptr || mesh.normal == nullptr || mesh.surface == nullptr) {
    log::warning("%s: mesh is missing vertex, normal or surface data", ATOM_FUNC_NAME);
    return;
  } else if (mesh.bone_weight == nullptr || mesh.bone_index == nullptr) {
    log::warning("%s: mesh is missing bone weight or bone index data", ATOM_FUNC_NAME);
    return;
  }

  VideoService &vs = context.video_processor;
  vs.enable_depth_test();
  context.uniforms.color = Vec3f(0.7, 0.7, 0.7);
  context.uniforms.sun_dir = Vec3f(0, 0, -1);
  context.uniforms.ambient_color = Vec3f(0.13, 0.13, 0.13);

  DrawCommand command;
  command.draw = DrawType::TRIANGLES;
  command.attributes[0] = mesh.vertex.get();
  command.types[0] = Type::VEC3F;
  command.attributes[1] = mesh.bone_index.get();
  command.types[1] = Type::VEC4U8;
  command.attributes[2] = mesh.bone_weight.get();
  command.types[2] = Type::VEC4F;
  command.indices = mesh.surface.get();
  command.program = &my_shader->program();
  vs.draw(command);
}

}
