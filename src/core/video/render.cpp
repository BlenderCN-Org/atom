#include "core/video/render.h"

#include "core/system/config.h"
#include "core/video/texture.h"
#include "core/video/video_service.h"
#include "core/video/draw_service.h"
#include "core/system/performance_counters.h"
#include "core/video/renderbuffer.h"
#include "core/math/transformation_stack.h"
#include "core/world/world.h"
#include "core/video/uniforms.h"
#include "core/video/render_context.h"

namespace atom {

void render_scene(VideoService &vs, DrawService &ds, const MeshTree &mesh_tree, const Camera &camera)
{
//  my_pc.start("Rendering");
  GL_ERROR_GUARD;
  Uniforms &u = vs.get_uniforms();
  RenderContext context = { u, vs, ds };

//  u.transformations.view = scene.camera->

//  u.eye_direction = camera.forward();
  u.transformations.view = camera.view;
  u.transformations.projection = camera.projection;

  vs.set_blending(BlendOperation::SRC_ALPHA, BlendOperation::ONE_MINUS_SRC_ALPHA);

  // render OPAQUE material
  for (const sptr<MeshTreeNode> &node : mesh_tree.all_nodes()) {
    if (node->material != nullptr && (node->mesh != nullptr || node->mesh_resource != nullptr)) {
      if (!node->visible)
        continue;

      const Mesh &me = node->mesh != nullptr ? *node->mesh : node->mesh_resource->mesh();

      const Material &m = node->material->material();
      u.transformations.model = node->transformations;
      u.model = u.transformations.model;
      u.mvp = u.transformations.model_view_projection();
      vs.set_draw_face(DrawFace::BOTH);
      vs.set_draw_face(m.face);
      node->material->material().draw_mesh(context,
        node->mesh_resource != nullptr ? node->mesh_resource->mesh() : *node->mesh);
    } else {
      log::warning("MeshTreeNode without Material or MeshBuffer");
    }
  }
//  my_pc.stop("Rendering");
}

}
