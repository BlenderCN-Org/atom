#include "trampoline.h"
#include <core/system/resource_service.h>
#include <core/audio/audio_service.h>
#include <core/utils/utils.h>
#include "game/constants.h"

namespace atom {
namespace game {

META_DEFINE_FIELDS(Trampoline) {
  FIELD(Trampoline, my_impulse, "impulse"),
  FIELD(Trampoline, my_countdown, "countdown")
};

META_DEFINE_CLASS(Trampoline, Entity, "Trampoline");

f32 Trampoline::WIDTH = 1;
f32 Trampoline::HEIGHT = 1;

uptr<Entity> Trampoline::create(World &world, Core &core)
{
  return uptr<Entity>(new Trampoline(world, core));
}

uptr<Entity> Trampoline::clone(World &world) const
{
  uptr<Trampoline> duplicate(new Trampoline(world, core()));
  copy_field_values(*this, *duplicate);
  return std::move(duplicate);
}

void Trampoline::on_init()
{
  set_size(WIDTH, HEIGHT);
  my_body.set_position(position());
  my_fixture.set_size(WIDTH, HEIGHT);
}

void Trampoline::on_update()
{
  if (my_fixture.collide()) {
    if (my_counter < 0) {
      my_counter = my_countdown;
    } else {
      my_counter -= stf(1.0);

      if (my_counter < 0) {
        // impulse
        const std::vector<FixtureComponent *> &contacts = my_fixture.contacts();

        if (!contacts.empty()) {
          contacts[0]->rigid_body().apply_impulse(my_impulse);
          core().audio_service().play(core().resource_service().get_sound_resource("trampoline_jump"));
        }
      }
    }
  } else {
    my_counter = -1;
  }
}

Trampoline::Trampoline(World &world, Core &core)
  : Entity(world, core, WIDTH, HEIGHT)
  , my_pixmap(*this)
  , my_body(RigidBodyType::KINEMATIC, *this)
  , my_fixture(Vec2f(WIDTH, HEIGHT), Vec2f(0, 0), my_body, *this)
  , my_impulse(0, 10)
  , my_countdown(1)
  , my_counter(-1)
{
  META_INIT();
  my_fixture.set_filter(CATEGORY_GROUND, MASK_GROUND);
  my_pixmap.set_material("trampoline");
  my_pixmap.set_auto_transform(true);
}

}
}
