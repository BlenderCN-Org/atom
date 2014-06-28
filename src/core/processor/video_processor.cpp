#include "core/processor/video_processor.h"

#include <algorithm>
#include "core/system/resource_service.h"

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
}

GBuffer& VideoProcessor::get_gbuffer()
{
  return my_gbuffer;
}

}
