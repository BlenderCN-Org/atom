#pragma once

#include <core/world/entity.h>
#include <core/video/mesh_tree_node.h>
#include <core/component/render_component.h>

namespace atom {

class TestObject : public Entity {
private:
  RenderComponent my_render;

public:
  static uptr<Entity> create(World &world, Core &core);

  TestObject(World &world, Core &core);
  ~TestObject();

  virtual uptr<Entity> clone(World &world) const override;

  void on_welcome() override;

  void on_goodbye() override;

  void on_init() override;

  void on_update() override;
};

}
