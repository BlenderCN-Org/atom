#pragma once

#include "core/world/entity.h"
#include "core/video/mesh_tree_node.h"

namespace atom {

class TestObject : public Entity {
private:
  sptr<MeshTreeNode> my_node;

public:
  static uptr<Entity> create(World &world, Core &core);

  TestObject(World &world, Core &core);

  virtual uptr<Entity> clone(World &world) const override;

  void on_welcome() override;

  void on_goodbye() override;

  void on_init() override;

  void on_update() override;
};

}
