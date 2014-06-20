#include "material.h"
#include "core/video/uniforms.h"
#include "core/video/render_context.h"
#include "core/video/video_service.h"
#include "core/video/mesh.h"
#include "core/system/resources.h"
#include "core/system/resource_service.h"

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
  return uptr<Material>(new FlatMaterial(rs.get_shader_resource("flat")));
}

FlatMaterial::FlatMaterial(
  const TechniqueResourcePtr &xshader)
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
  program.set_param("color", color);

  vs.bind_attribute(0, *vertices, Type::VEC3F);
  vs.draw_index_array(GL_TRIANGLES, *indices, indices->size() / sizeof(u32));
  vs.draw_arrays(GL_TRIANGLES, 0, vertices->size() / sizeof(Vec3f));
  vs.unbind_vertex_attribute(0);
}

//-----------------------------------------------------------------------------
//
// Pixmap Material
//
//-----------------------------------------------------------------------------

META_DEFINE_FIELDS(PixmapMaterial) {
  FIELD(PixmapMaterial, my_shader, "shader"),
  FIELD(PixmapMaterial, my_texture, "texture"),
  FIELD(PixmapMaterial, my_color, "color"),
};

META_DEFINE_CLASS(PixmapMaterial, Material, "PixmapMaterial");


uptr<Material> PixmapMaterial::create(ResourceService &rs)
{
  return uptr<Material>(new PixmapMaterial(rs.video_service(), rs.get_shader_resource("pixmap")));
}

PixmapMaterial::PixmapMaterial(VideoService &vs, const TechniqueResourcePtr &pixmap_shader)
  : my_sampler(TextureFilter::NEAREST, TextureWrap::REPEAT)
  , my_program(vs)
{
  META_INIT();

  Shader vertex_shader(vs, VideoShaderType::VERTEX);
  Shader pixel_shader(vs, VideoShaderType::PIXEL);
  String vertex_src, pixel_src;

  utils::load_file_into_string("data/shader/pixmap.vs", vertex_src);
  utils::load_file_into_string("data/shader/pixmap.ps", pixel_src);


  if (!vertex_shader.compile(vertex_src)) {
    error("Error while compiling vertex shader");
  }

  if (!pixel_shader.compile(pixel_src)) {
    error("Error while compiling pixel shader");
  }

  if (!my_program.link(pixel_shader, vertex_shader)) {
    error("Error while linking program");
  }
  my_program.locate_uniforms();
}

PixmapMaterial::~PixmapMaterial()
{
}

void PixmapMaterial::draw_mesh(const RenderContext &context, const Mesh &mesh)
{
  assert(my_shader != nullptr);
  const VideoBuffer *vb = mesh.find_stream(StreamId::VERTEX);
  const VideoBuffer *ub = mesh.find_stream(StreamId::UV);

  if (vb == nullptr || ub == nullptr) {
    log::warning("This mesh doesn't contain vertex or uv stream");
    return;
  }


  VideoService &vs = context.video_processor;

  vs.bind_program(my_program);
  my_program.set_param("model_view_projection", context.uniforms.transformations.model_view_projection());
  my_program.set_param("factor", my_color);

  vs.bind_attribute(0, *vb, Type::VEC3F);
  vs.bind_attribute(1, *ub, Type::VEC2F);
  vs.bind_texture(0, my_texture->texture());//, shader.uniform_pixmap_texture());
  vs.bind_sampler(0, my_sampler);

  vs.draw_arrays(GL_TRIANGLES, 0, vb->size() / sizeof(Vec3f));
  vs.unbind_sampler(0);
  vs.unbind_texture(0);
  vs.unbind_vertex_attribute(0);
  vs.unbind_vertex_attribute(1);
}

}
