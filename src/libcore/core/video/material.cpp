#include "material.h"
#include "../system/resources.h"
#include "../system/resource_service.h"
#include "uniforms.h"
#include "render_context.h"
#include "video_service.h"
#include "mesh.h"

namespace atom {

META_CLASS(Material,
  FIELD(my_face, "face")
)

Material::~Material()
{
}


//
// SimpleMaterial
//

META_CLASS(SimpleMaterial,
  FIELD(my_flags, "flags"),
  FIELD(my_draw_face, "draw_face"),
  FIELD(my_draw_type, "draw_type"),
  FIELD(my_fill_mode, "fill_mode"),
  FIELD(my_depth_test, "depth_test"),
  FIELD(my_technique, "shader")
)

uptr<atom::SimpleMaterial::Material> SimpleMaterial::create(ResourceService &rs)
{
  return uptr<Material>(new SimpleMaterial(DrawFlags::VERTEX));
}

SimpleMaterial::SimpleMaterial(u32 draw_flags)
  : my_flags(draw_flags)
  , my_draw_face(DrawFace::FRONT)
  , my_draw_type(DrawType::NONE)
  , my_fill_mode(FillMode::FILL)
  , my_depth_test(true)
{
  META_INIT();
}

SimpleMaterial::~SimpleMaterial()
{
  // empty
}

void SimpleMaterial::draw_mesh(const RenderContext &context, const Mesh &mesh)
{
  if (my_technique == nullptr) {
    log::warning("%s: no shader", ATOM_FUNC_NAME);
    return;
  }

  DrawCommand command;

  if (my_flags & DrawFlags::VERTEX) {
    if (mesh.vertex == nullptr) {
      log::warning("%s: mesh missing vertex data", ATOM_FUNC_NAME);
      return;
    }

    command.attributes[0] = mesh.vertex.get();
    command.types[0] = Type::VEC3F;
  }

  if (my_flags & DrawFlags::NORMAL) {
    if (mesh.normal == nullptr) {
      log::warning("%s: mesh missing normal data", ATOM_FUNC_NAME);
      return;
    }
    command.attributes[1] = mesh.normal.get();
    command.types[1] = Type::VEC3F;
  }

  if (my_flags & DrawFlags::COLOR) {
    if (mesh.color == nullptr) {
      log::warning("%s: mesh missing color data", ATOM_FUNC_NAME);
      return;
    }
    command.attributes[2] = mesh.color.get();
    command.types[2] = Type::VEC3F;
  }

  if (my_flags & DrawFlags::INDEX) {
    if (mesh.surface == nullptr) {
      log::warning("%s: mesh missing index data", ATOM_FUNC_NAME);
      return;
    }
    command.indices = mesh.surface.get();
  }

  command.draw = my_draw_type;
  command.face = my_draw_face;
  command.depth_test = my_depth_test;
  command.program = &my_technique->program();
  context.video_processor.draw(command);
}


//
// DebugMaterial (wireframe)
//

uptr<Material> DebugMaterial::create(ResourceService &rs)
{
  return uptr<Material>(new DebugMaterial());
}

DebugMaterial::DebugMaterial()
  : SimpleMaterial(DrawFlags::VERTEX | DrawFlags::COLOR)
{

}


//
// Flat Material
//

META_CLASS(FlatMaterial,
  FIELD(my_color, "color")
)

uptr<Material> FlatMaterial::create(ResourceService &rs)
{
  return uptr<Material>(new FlatMaterial());
}

FlatMaterial::FlatMaterial()
  : SimpleMaterial(DrawFlags::VERTEX | DrawFlags::INDEX)
  , my_color(1, 0, 0)
{
  META_INIT();
}

FlatMaterial::~FlatMaterial()
{
  // empty
}

void FlatMaterial::draw_mesh(const RenderContext &context, const Mesh &mesh)
{
  context.uniforms.color = my_color;
  SimpleMaterial::draw_mesh(context, mesh);
}


//
// PhongMaterial
//

META_CLASS(PhongMaterial,
  FIELD(my_color, "color")
)

uptr<Material> PhongMaterial::create(ResourceService &rs)
{
  return uptr<Material>(new PhongMaterial(rs.get_technique("phong")));
}

PhongMaterial::PhongMaterial(const TechniqueResourcePtr &shader)
  : SimpleMaterial(DrawFlags::VERTEX | DrawFlags::NORMAL | DrawFlags::INDEX)
{
  META_INIT();
}

PhongMaterial::~PhongMaterial()
{
}

void PhongMaterial::draw_mesh(const RenderContext &context, const Mesh &mesh)
{
  context.uniforms.color = my_color;
  context.uniforms.sun_dir = Vec3f(0, 0, -1);
  context.uniforms.ambient_color = Vec3f(0.13, 0.13, 0.13);
  SimpleMaterial::draw_mesh(context, mesh);
}


//-----------------------------------------------------------------------------
//
// Skin Material
//
//-----------------------------------------------------------------------------

META_CLASS(FlatSkinMaterial,
  FIELD(my_shader, "shader"),
  FIELD(my_color, "color")
)

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
