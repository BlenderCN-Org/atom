#include "player.h"
#include <core/world/world.h>
#include <core/math/math.h>
#include <core/processor/physics_processor.h>
#include <core/system/resources.h>
#include <core/system/resource_service.h>
#include <core/audio/audio_service.h>
#include <core/input/input_service.h>
#include <core/utils/b2_utils.h>
#include <core/video/technique.h>
#include <core/video/shader.h>
#include "game/constants.h"

namespace atom {
namespace game {

const f32 CLIMB_RATE = 5;
const f32 ROPE_LENGTH = 5;
const f32 POINTER_LENGTH = 0.9;
const f32 PLAYER_WIDTH = 1;
const f32 PLAYER_HEIGHT = 1.6f;

uptr<Entity> Player::create(World &world, Core &core)
{
  return uptr<Entity>(new Player(world, core));
}

Player::Player(World &world, Core &core)
  : Entity(world, core)
  , my_state(State::IDLE)
  , my_body(RigidBodyType::DYNAMIC, *this)
  , my_body_collider(Vec2f(PLAYER_WIDTH, PLAYER_HEIGHT), Vec2f(0, 0), my_body, *this)
  , my_ground_collider(Vec2f(0.8f, 0.2f), Vec2f(0, -2), my_body, *this)
  , my_pixmap(*this)
  , my_pointer(*this)
  , my_rope(my_body, *this)
  , my_walking_track_id(AudioService::INVALID_ID)
  , my_direction(Direction::RIGHT)
{
  set_size(PLAYER_WIDTH, PLAYER_HEIGHT);
  my_pointer.set_auto_transform(false);
  my_pointer.set_size(Vec2f(0.1, 0.1));
  my_pointer.set_material("pointer");
  my_pixmap.set_material("player");
  my_pixmap.set_size(Vec2f(PLAYER_WIDTH, PLAYER_HEIGHT));
  my_body.set_fixed_rotation(true);
  my_body.set_update_entity_position(false);
  my_body_collider.set_filter(CATEGORY_PLAYER, MASK_PLAYER);
  my_ground_collider.set_sensor(true);
  my_ground_collider.set_filter(CATEGORY_PLAYER, MASK_PLAYER);
  my_rope.set_length(ROPE_LENGTH);
}

uptr<Entity> Player::clone(World &world) const
{
  uptr<Player> duplicate(new Player(world, core()));
  copy_field_values(*this, *duplicate);
  return std::move(duplicate);
}

void Player::on_init()
{
  my_body.set_position(position());
  my_pixmap.set_model("ppp");
}

void Player::on_update()
{
  InputService &is = core().input_service();

  set_position(my_body.position());

  f32 angle = atan2(is.mouse().position.y, is.mouse().position.x);
  my_pointer.set_transform(position() + Vec2f(std::cos(angle), std::sin(angle)) * ROPE_LENGTH * POINTER_LENGTH);

  int direction = 0;



  Vec2f rope_direction(cos(angle) * ROPE_LENGTH, sin(angle) * ROPE_LENGTH);
  NearestRayCast ray(CATEGORY_STICKER);
  world().processors().physics.ray_cast(position(), rope_direction, ray);


  bool collide_with_sticker = ray.nearest_fixture() != nullptr;

//    bool can_jump = my_ground_collider.collide();

  if (is.is_key_pressed(Key::KEY_LEFT)) {
    direction -= 1;
  }

  if (is.is_key_pressed(Key::KEY_RIGHT)) {
    direction += 1;
  }

  if (my_rope.is_tied()) {
    f32 length = my_rope.length();
    bool change = false;

    if (is.is_key_pressed(Key::KEY_UP)) {
      length = length - stf(CLIMB_RATE);
      change = true;
    }

    if (is.is_key_pressed(Key::KEY_DOWN)) {
      length = length + stf(CLIMB_RATE);
      change = true;
    }

    if (change) {
      my_rope.set_length(range(0.0f, ROPE_LENGTH, length));
    }
  }

  Vec2f final_velocity(0, 0);

  const std::vector<FixtureComponent *> &contacts = my_ground_collider.contacts();

  for (FixtureComponent *f : contacts) {
    if (f->category() == CATEGORY_GROUND) {
      final_velocity += f->rigid_body().linear_velocity();
    }
  }

  if (my_ground_collider.collide() && !my_rope.is_tied()) {
    Vec2f v = my_body.linear_velocity();
    final_velocity += Vec2f((5 * direction - v.x) * my_body.mass(), 0);

    my_body.apply_impulse(final_velocity);
  } else if (direction != 0) {
    my_body.apply_force(Vec2f(8 * direction, 0));
  }

  if (my_ground_collider.collide() && is.is_key_down(Key::KEY_UP)) {
    my_body.apply_impulse(Vec2f(0, my_body.mass() * 10));
  }

  if (my_ground_collider.collide()) {
    if (direction != 0) {
      switch_state(State::WALKING);
    } else {
      switch_state(State::IDLE);
    }
  } else {
    switch_state(State::JUMPING);
  }


  if (is.is_key_down(Key::KEY_LMB)) {
    if (my_rope.is_tied()) {
      my_rope.cut();
      my_pointer.set_visible(true);
    } else if (collide_with_sticker){
      my_rope.set_length(ROPE_LENGTH);
      my_rope.tie(position() + Vec2f(cos(angle), sin(angle)) * ROPE_LENGTH * POINTER_LENGTH);
      my_pointer.set_visible(false);
    }
  }

  if (direction != 0) {
    my_direction = direction < 0 ? Direction::LEFT : Direction::RIGHT;
  }

  my_pixmap.set_invert_horizontal(my_direction == Direction::LEFT);
}

void Player::switch_state(Player::State state)
{
  if (state == my_state) {
    return;
  }

//  State old_state = my_state;
  my_state = state;

  switch (my_state) {
    case State::WALKING:
      my_walking_track_id = core().audio_service().play(core().resource_service().get_sound_resource("walking"), true);
      break;

    default:
      if (my_walking_track_id != AudioService::INVALID_ID) {
        core().audio_service().stop(my_walking_track_id);
        my_walking_track_id = AudioService::INVALID_ID;
      }
  }
}

}
}
