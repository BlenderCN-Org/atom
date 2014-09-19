#pragma once

#include "../corefwd.h"
#include "../noncopyable.h"
#include "../video/mesh_tree.h"
#include "../video/gbuffer.h"

namespace atom {

typedef std::vector<RenderComponent *> RenderComponentArray;

class RenderProcessor : private NonCopyable {
  VideoService        &my_vs;
  ResourceService     &my_rs;
  GBuffer              my_gbuffer;
  MeshTree             my_mesh_tree;
  RenderComponentArray my_components;

public:
  RenderProcessor(VideoService &vs, ResourceService &rs);
  ~RenderProcessor();

  void set_resolution(int width, int height);

  MeshTree* mesh_tree();

  MeshTree* gui_tree();

  void render(const Camera &camera);

  GBuffer& get_gbuffer();

  void register_component(RenderComponent *component);
  void unregister_component(RenderComponent *component);
};

}
