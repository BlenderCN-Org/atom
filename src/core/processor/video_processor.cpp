#include "core/processor/video_processor.h"

#include <algorithm>
#include "core/system/resource_service.h"
#include "core/component/render_component.h"

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

void VideoProcessor::poll()
{
  for (RenderComponent *component : my_components) {
    component->update_transform();
  }
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
