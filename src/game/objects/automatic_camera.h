#pragma once

#include <core/world/entity.h>

namespace atom {
namespace game {

class AutomaticCamera : public Entity {
public:
  AutomaticCamera(World &world, Core &core);

  void on_welcome() override;

  void on_goodbye() override;

  void on_init() override;

  void on_update() override;

  uptr<Entity> clone(World &world) const override;

private:
  bool         my_game_started;
  sptr<Entity> my_player;
};

uptr<Entity> create_automatic_camera(World &world, Core &core);

}
}
