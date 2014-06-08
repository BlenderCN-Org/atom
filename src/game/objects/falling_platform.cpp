#include "falling_platform.h"
#include <core/audio/audio_service.h>
#include <core/system/resource_service.h>
#include "game/constants.h"

namespace atom {
namespace game {

const f32 FallingPlatform::WIDTH = 1.5;
const f32 FallingPlatform::HEIGHT = 1;

//META_DEFINE_FIELDS(FallingPlatform) {
//  FIELD(SimpleMoveablePlatform, my_length, "length")
//};

//META_DEFINE_CLASS(FallingPlatform, Entity, "SimpleFallingPlatform");

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

uptr<Entity> FallingPlatform::create(World &world, Core &core)
{
  return uptr<Entity>(new FallingPlatform(world, core));
}

FallingPlatform::FallingPlatform(World &world, Core &core)
  : Entity(world, core)
  , my_state(State::IDLE)
  , my_fall_start(-1)
  , my_pixmap(*this)
  , my_body(RigidBodyType::KINEMATIC, *this)
  , my_fixture(Vec2f(1, 1), Vec2f(0, 0), my_body, *this)
{
  my_fixture.set_filter(CATEGORY_GROUND, MASK_GROUND);
  my_pixmap.set_material("platform");
  my_pixmap.set_auto_transform(true);
}

void FallingPlatform::on_init()
{
  set_size(WIDTH, HEIGHT);
  my_body.set_position(position());
  my_fixture.set_size(WIDTH, HEIGHT - 0.2);
  my_pixmap.set_uv(Vec2f(0, 0), Vec2f(WIDTH/ 2, 1));
  my_pixmap.set_size(Vec2f(WIDTH, HEIGHT));
}

void FallingPlatform::on_update()
{
  if (my_fall_start < 0) {
    if (my_fixture.collide()) {
      my_fall_start = world().time();
    }
  } else {
    f32 t = world().time() - my_fall_start;

    if (position().y < -100) {  // zastavim sa ak som uz dost nizko, kde ma hrac nezbada
      my_body.set_linear_velocity(Vec2f(0, 0));
    } else if (t > 1) { // po 2s zacinam padat
      switch_state(State::FALLING);
      f32 velocity = std::min(t * 10, 10.0f);
      my_body.set_linear_velocity(Vec2f(0, -velocity));
    }
  }

  set_position(my_body.position());
}

uptr<Entity> FallingPlatform::clone(World &world) const
{
  uptr<FallingPlatform> duplicate(new FallingPlatform(world, core()));
  copy_field_values(*this, *duplicate);
  return std::move(duplicate);
}

void FallingPlatform::switch_state(FallingPlatform::State state)
{
  if (my_state == state) {
    return;
  }

  my_state = state;

  if (my_state == State::FALLING) {
    core().audio_service().play(core().resource_service().get_sound_resource("falling_platform"));
  }
}

}
}
