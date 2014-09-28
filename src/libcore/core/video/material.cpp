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


//
// LinesMaterial
//

META_DEFINE_FIELDS(LinesMaterial) {
  FIELD(LinesMaterial, color, "color")
};

META_DEFINE_CLASS(LinesMaterial, Material, "LinesMaterial");

uptr<Material> LinesMaterial::create(ResourceService &rs)
{
  return uptr<Material>(new LinesMaterial(rs.get_technique("lines")));
}

LinesMaterial::LinesMaterial(const TechniqueResourcePtr &shader)
  : my_shader(shader)
{
  META_INIT();
}

LinesMaterial::~LinesMaterial()
{
  // empty
}

void LinesMaterial::draw_mesh(const RenderContext &context, const Mesh &mesh)
{
  assert(my_shader != nullptr);

  if (mesh.vertex == nullptr) {
    log::warning("%s: mesh missing vertex data", ATOM_FUNC_NAME);
    return;
  }

  context.uniforms.color = color;

  DrawCommand command;
  command.draw = DrawType::LINES;
  command.attributes[0] = mesh.vertex.get();
  command.types[0] = Type::VEC3F;
  command.program = &my_shader->program();
  context.video_processor.draw(command);
}


//
// Flat Material
//

META_DEFINE_FIELDS(FlatMaterial) {
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
  // empty
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
  command.draw = DrawType::TRIANGLES;
  command.attributes[0] = mesh.vertex.get();
  command.types[0] = Type::VEC3F;
  command.indices = mesh.surface.get();
  command.program = &my_shader->program();
  context.video_processor.draw(command);
}


//
// WireframeMaterial
//

META_DEFINE_FIELDS(WireframeMaterial) {
  FIELD(WireframeMaterial, color, "color")
};

META_DEFINE_CLASS(WireframeMaterial, Material, "WireframeMaterial");

uptr<Material> WireframeMaterial::create(ResourceService &rs)
{
  return uptr<Material>(new WireframeMaterial(rs.get_technique("lines")));
}

WireframeMaterial::WireframeMaterial(const TechniqueResourcePtr &shader)
  : my_shader(shader)
{
  META_INIT();
}

WireframeMaterial::~WireframeMaterial()
{
  // empty
}

void WireframeMaterial::draw_mesh(const RenderContext &context, const Mesh &mesh)
{
  assert(my_shader != nullptr);
  not_tested();

  if (mesh.vertex == nullptr || mesh.surface == nullptr) {
    log::warning("%s: mesh missing vertex or surface data", ATOM_FUNC_NAME);
    return;
  }

  context.uniforms.color = color;

  DrawCommand command;
  command.draw = DrawType::TRIANGLES;
  command.attributes[0] = mesh.vertex.get();
  command.types[0] = Type::VEC3F;
  command.indices = mesh.surface.get();
  command.program = &my_shader->program();
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);  // TODO: vyriesit v ramci materialu, nie OpenGL
  context.video_processor.draw(command);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}


//
// PhongMaterial
//

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

  if (mesh.vertex == nullptr || mesh.normal == nullptr || mesh.surface == nullptr) {
    log::warning("%s: mesh missing vertex, normal or surface data", ATOM_FUNC_NAME);
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
  command.attributes[1] = mesh.normal.get();
  command.types[1] = Type::VEC3F;
  command.indices = mesh.surface.get();
  command.program = &my_shader->program();
  context.video_processor.draw(command);
}


//-----------------------------------------------------------------------------
//
// Skin Material
//
//-----------------------------------------------------------------------------

META_DEFINE_FIELDS(FlatSkinMaterial) {
  FIELD(FlatSkinMaterial, my_shader, "shader"),
  FIELD(FlatSkinMaterial, my_color, "color"),
};

META_DEFINE_CLASS(FlatSkinMaterial, Material, "FlatSkinMaterial");


uptr<Material> FlatSkinMaterial::create(ResourceService &rs)
{
  return uptr<Material>(new FlatSkinMaterial(rs.get_technique("flat_skin")));
}

FlatSkinMaterial::FlatSkinMaterial(const TechniqueResourcePtr &shader)
  : my_program(shader)
{
  META_INIT();
}

FlatSkinMaterial::~FlatSkinMaterial()
{
}

void FlatSkinMaterial::draw_mesh(const RenderContext &context, const Mesh &mesh)
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
