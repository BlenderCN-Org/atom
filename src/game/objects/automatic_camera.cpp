#include "automatic_camera.h"
#include <core/world/world.h>

namespace atom {
namespace game {

AutomaticCamera::AutomaticCamera(World &world, Core &core)
  : Entity(world, core, 1, 1)
  , my_game_started(false)
{

}

void AutomaticCamera::on_welcome()
{

}

void AutomaticCamera::on_goodbye()
{
  my_player.reset();
}

void AutomaticCamera::on_init()
{
  my_game_started = true;
}

void AutomaticCamera::on_update()
{
  if (!my_game_started) {
    return;
  }

  set_position(position() + Vec2f(0.05, 0));

  if (my_player == nullptr) {
    my_player = world().find_entity("player");
  }

  if (my_player == nullptr) {
    return;
  }

  const Config &config = Config::instance();

  const f32 cw = 50;
  const f32 ch = cw / config.get_aspect_ratio();

  Vec2f player_position = my_player->position();
  Camera camera;
  camera.set_position(player_position.x, player_position.y, 10);
  camera.set_orthographic(-cw / 2, cw / 2, -ch / 2, ch / 2, -10, 10);

  world().set_camera(camera);
}

uptr<Entity> AutomaticCamera::clone(World &world) const
{
  return uptr<Entity>(new AutomaticCamera(world, core()));
}

uptr<Entity> create_automatic_camera(World &world, Core &core)
{
  return uptr<Entity>(new AutomaticCamera(world, core));
}

}
}
