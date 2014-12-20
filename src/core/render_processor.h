#pragma once

#include "processor.h"
#include "mesh_tree.h"
#include "gbuffer.h"

namespace atom {

typedef std::vector<RenderComponent *> RenderComponentArray;

class RenderProcessor : public NullProcessor {
  GBuffer              my_gbuffer;
  MeshTree             my_mesh_tree;
  RenderComponentArray my_components;

public:
  explicit RenderProcessor(World &world);
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
