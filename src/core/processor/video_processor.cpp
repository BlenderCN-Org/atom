#include "core/processor/video_processor.h"

#include <algorithm>
#include "core/system/resource_service.h"
#include "core/component/render_component.h"
#include "core/video/render_context.h"
#include "core/video/uniforms.h"
#include "core/math/camera.h"

namespace atom {

VideoProcessor::VideoProcessor(VideoService &vs, ResourceService &rs)
  : my_vs(vs)
  , my_rs(rs)
  , my_gbuffer(vs)
{
}

VideoProcessor::~VideoProcessor()
{
  // empty
}

void VideoProcessor::set_resolution(int width, int height)
{
  my_gbuffer.set_resolution(width, height);
}

MeshTree* VideoProcessor::mesh_tree()
{
  return &my_mesh_tree;
}

void VideoProcessor::render(const Camera &camera)
{
  //  my_pc.start("Rendering");
  GL_ERROR_GUARD;
  Uniforms &u = my_vs.get_uniforms();
  RenderContext context = { u, my_vs };
  
  u.transformations.view = camera.view;
  u.transformations.projection = camera.projection;
  
  my_vs.set_blending(BlendOperation::SRC_ALPHA, BlendOperation::ONE_MINUS_SRC_ALPHA);
    
  for (RenderComponent *component : my_components) {
    const MaterialResourcePtr &material = component->material();
    const MeshResourcePtr &mesh = component->mesh();
      
    if (material == nullptr || mesh == nullptr) {
      log::warning("RenderComponent without mesh or material");
      continue;
    }
      
    u.transformations.model = component->entity().transform();
    u.model = u.transformations.model;
    u.mvp = u.transformations.model_view_projection();
    my_vs.set_draw_face(DrawFace::BOTH);
    my_vs.set_draw_face(material->material().face);
    log::info("Rendering obje");
    material->material().draw_mesh(context, mesh->mesh());
  }
  
  //  my_pc.stop("Rendering");
}

GBuffer& VideoProcessor::get_gbuffer()
{
  return my_gbuffer;
}

void VideoProcessor::register_component(RenderComponent *component)
{
  assert(component != nullptr);
  my_components.push_back(component);
}

void VideoProcessor::unregister_component(RenderComponent *component)
{
  assert(component != nullptr);
  my_components.erase(std::remove(my_components.begin(), my_components.end(),
     component), my_components.end());
}

}
