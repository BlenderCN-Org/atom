#pragma once

#include "core/component/component.h"
#include "core/video/material.h"

namespace atom {

class RenderComponent : public Component {
  MeshResourcePtr my_mesh;
  MaterialResourcePtr my_material;
  sptr<MeshTreeNode> my_node;

public:
  explicit RenderComponent(Entity &entity, const MeshResourcePtr &mesh,
    const MaterialResourcePtr &material);

  void attach() override;
  void detach() override;

  void update_transform();
};

}
