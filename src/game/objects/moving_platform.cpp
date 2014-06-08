#include "moving_platform.h"
#include "game/constants.h"


namespace atom {
namespace game {



const f32 MovingPlatform::WIDTH = 3;
const f32 MovingPlatform::HEIGHT = 1;

//META_DEFINE_FIELDS(MovingPlatform) {
//  FIELD(SimpleMoveablePlatform, my_length, "length")
//};

//META_DEFINE_CLASS(MovingPlatform, Entity, "SimpleMoveablePlatform");

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

uptr<Entity> MovingPlatform::create(World &world, Core &core)
{
  return uptr<Entity>(new MovingPlatform(world, core));
}

MovingPlatform::MovingPlatform(World &world, Core &core)
  : Entity(world, core)
  , my_pixmap(*this)
  , my_body(RigidBodyType::KINEMATIC, *this)
  , my_fixture(Vec2f(1, 1), Vec2f(0, 0), my_body, *this)
{
  META_INIT();
  my_fixture.set_filter(CATEGORY_GROUND, MASK_GROUND);
  my_pixmap.set_material("platform");
  my_pixmap.set_auto_transform(true);
}

void MovingPlatform::on_init()
{
  set_size(WIDTH, HEIGHT);
  my_body.set_position(position());
  my_fixture.set_size(WIDTH, HEIGHT - 0.2);
  my_pixmap.set_uv(Vec2f(0, 0), Vec2f(WIDTH/ 2, 1));
  my_pixmap.set_size(Vec2f(WIDTH, HEIGHT));
}

void MovingPlatform::on_update()
{
  f32 velocity = sin(world().time() * 2);
  my_body.set_linear_velocity(3.0f * Vec2f(velocity, 0));
  set_position(my_body.position());
}

uptr<Entity> MovingPlatform::clone(World &world) const
{
  uptr<MovingPlatform> duplicate(new MovingPlatform(world, core()));
  copy_field_values(*this, *duplicate);
  return std::move(duplicate);
}

}
}
