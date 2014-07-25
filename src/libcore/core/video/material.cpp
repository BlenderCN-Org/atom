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
  FIELD(FlatMaterial, shader, "shader"),
  FIELD(FlatMaterial, color, "color")
};

META_DEFINE_CLASS(FlatMaterial, Material, "FlatMaterial");

uptr<Material> FlatMaterial::create(ResourceService &rs)
{
  return uptr<Material>(new FlatMaterial(rs.get_technique("flat")));
}

FlatMaterial::FlatMaterial(const TechniqueResourcePtr &xshader)
  : shader(xshader)
{
  META_INIT();
}

FlatMaterial::~FlatMaterial()
{
}

void FlatMaterial::draw_mesh(const RenderContext &context, const Mesh &mesh)
{
  assert(shader != nullptr);

  const VideoBuffer *vertices = mesh.find_stream(StreamId::VERTEX);
  const VideoBuffer *indices = mesh.find_stream(StreamId::INDEX);

  if (vertices == nullptr) {
    log::warning("This mesh doesn't have vertex stream");
    return;
  }

  if (indices == nullptr) {
    log::warning("This mesh doesn't have index stream");
    return;
  }

  Technique &program = shader->program();
  VideoService &vs = context.video_processor;
  vs.bind_program(program);
  program.set_param("model_view_projection", context.uniforms.transformations.model_view_projection());
  context.uniforms.color = color;
//  program.set_param("color", color);

  MetaObject properties = meta_object(context.uniforms);
  program.pull(properties);

  vs.bind_attribute(0, *vertices, Type::VEC3F);
  vs.draw_index_array(GL_TRIANGLES, *indices, indices->size() / sizeof(u32));
  vs.draw_arrays(GL_TRIANGLES, 0, vertices->size() / sizeof(Vec3f));
  vs.unbind_vertex_attribute(0);
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
  const VideoBuffer *vb = mesh.find_stream(StreamId::VERTEX);
  const VideoBuffer *nb = mesh.find_stream(StreamId::NORMAL);
  const VideoBuffer *ib = mesh.find_stream(StreamId::INDEX);

  if (vb == nullptr || nb == nullptr || ib == nullptr) {
    log::warning("This mesh doesn't contain vertex, normal or index stream");
    return;
  }


  VideoService &vs = context.video_processor;
  vs.enable_depth_test();

  Technique &program = my_program->program();
  vs.bind_program(program);
  context.uniforms.color = Vec3f(0.7, 0.7, 0.7);
  context.uniforms.sun_dir = Vec3f(0, 0, -1);
  context.uniforms.ambient_color = Vec3f(0.13, 0.13, 0.13);
//  program.set_param("model_view_projection", context.uniforms.transformations.model_view_projection());
  program.pull(meta_object(context.uniforms));

  vs.bind_attribute(0, *vb, Type::VEC3F);
  vs.bind_attribute(1, *nb, Type::VEC3F);
  vs.draw_index_array(GL_TRIANGLES, *ib, ib->size() / 3);
  vs.unbind_vertex_attribute(0);
  vs.unbind_vertex_attribute(1);
}

}
