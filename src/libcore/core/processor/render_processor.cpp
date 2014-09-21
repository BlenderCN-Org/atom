#include "render_processor.h"

#include <algorithm>
#include "../system/resource_service.h"
#include "../component/render_component.h"
#include "../component/skeleton_component.h"
#include "../video/render_context.h"
#include "../video/uniforms.h"
#include "../math/camera.h"

namespace atom {

RenderProcessor::RenderProcessor(World &world)
  : NullProcessor(world)
  , my_gbuffer(world.core().video_service())
{
}

RenderProcessor::~RenderProcessor()
{
  // empty
}

void RenderProcessor::set_resolution(int width, int height)
{
  my_gbuffer.set_resolution(width, height);
}

MeshTree* RenderProcessor::mesh_tree()
{
  return &my_mesh_tree;
}

void RenderProcessor::render(const Camera &camera)
{
  //  my_pc.start("Rendering");
  GL_ERROR_GUARD;

  VideoService &vs = core().video_service();

  Uniforms &u = vs.get_uniforms();
  RenderContext context = { u, vs };

  u.transformations.view = camera.view;
  u.transformations.projection = camera.projection;

  vs.set_blending(BlendOperation::SRC_ALPHA, BlendOperation::ONE_MINUS_SRC_ALPHA);

  for (RenderComponent *component : my_components) {
    if (!component->is_enabled()) {
      continue;
    }

    Entity &entity = component->entity();

    const MaterialResourcePtr &material = component->material();
    const MeshResourcePtr &mesh = component->mesh();

    if (material == nullptr || mesh == nullptr) {
      log::warning("RenderComponent without mesh or material");
      continue;
    }

    u.transformations.model = entity.transform();
    u.model = u.transformations.model;
    u.mvp = u.transformations.model_view_projection();

    SkeletonComponent *skeleton = entity.find_component<SkeletonComponent>();

    if (skeleton != nullptr) {
      int i = 0;
      for (const Mat4f &t: skeleton->get_transforms()) {
        u.bones[i++] = t;
      }
    }

//    my_vs.set_draw_face(DrawFace::BOTH);
    vs.set_draw_face(material->material().face);
    material->material().draw_mesh(context, mesh->mesh());
  }

  //  my_pc.stop("Rendering");
}

GBuffer& RenderProcessor::get_gbuffer()
{
  return my_gbuffer;
}

void RenderProcessor::register_component(RenderComponent *component)
{
  assert(component != nullptr);
  my_components.push_back(component);
}

void RenderProcessor::unregister_component(RenderComponent *component)
{
  assert(component != nullptr);
  my_components.erase(std::remove(my_components.begin(), my_components.end(),
     component), my_components.end());
}

}
