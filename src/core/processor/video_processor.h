#pragma once

#include "core/corefwd.h"
#include "core/noncopyable.h"
#include "core/video/mesh_tree.h"
#include "core/video/gbuffer.h"

namespace atom {

class VideoProcessor : private NonCopyable {
public:
  VideoProcessor(VideoService &vs, ResourceService &rs);
  ~VideoProcessor();

  void set_resolution(int width, int height);

  void add_pixmap(PixmapComponent *component);

  void remove_pixmap(PixmapComponent *component);

  MeshTree* mesh_tree();

  MeshTree* gui_tree();

  void poll();

  GBuffer& get_gbuffer();

private:
  VideoService    &my_vs;
  ResourceService &my_rs;
  GBuffer          my_gbuffer;
  MeshTree         my_mesh_tree;
  std::vector<PixmapComponent *> my_pixmap_components;
};

}
