#pragma once

#include <core/world/entity.h>

namespace atom {
namespace game {

class Soldier : public Entity {
public:
  static uptr<Entity> create(World &world, Core &core);

  Soldier(World &world, Core &core);

  uptr<Entity> clone(World &world) const override;

  void on_init() override;

  void on_update() override;


};

}
}
