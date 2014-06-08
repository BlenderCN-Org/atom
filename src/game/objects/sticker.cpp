#include "sticker.h"
#include <core/world/entity.h>
#include <core/component/pixmap_component.h>
#include <core/component/sensor_component.h>
#include <core/component/rigid_body_component.h>
#include <core/component/fixture_component.h>
#include <core/component/joint_component.h>
#include "game/constants.h"

namespace atom {
namespace game {

class Sticker : public Entity {
//  META_DECLARE_CLASS;

  PixmapComponent my_pixmap;
  RigidBodyComponent my_body;
  BoxFixtureComponent my_fixture;
  FixedJointComponent my_hook;
  bool                my_is_fixed;

public:
  Sticker(World &world, Core &core)
    : Entity(world, core)
    , my_pixmap(*this)
//    , my_sensor(*this)
    , my_body(RigidBodyType::DYNAMIC, *this)
    , my_fixture(Vec2f(1, 1), Vec2f(0, 0), my_body, *this)
    , my_hook(my_body, *this)
    , my_is_fixed(false)
  {
//    META_INIT();
    my_fixture.set_filter(CATEGORY_STICKER, MASK_STICKER);
//    my_fixture.set_sensor(true);
  }

  void on_init() override
  {
    my_body.set_position(position());
    my_hook.fix(position());
  }

  void on_update() override
  {
    if (!my_is_fixed) {
      my_hook.fix(position());
      my_is_fixed = true;
    }
  }

  uptr<Entity> clone(World &world) const override
  {
    uptr<Entity> duplicate(new Sticker(world, core()));
    copy_field_values(*this, *duplicate);
    return duplicate;
  }

//  SensorComponent my_sensor;
};

//META_DEFINE_FIELDS(Sticker) {

//};

//META_DEFINE_CLASS(Sticker, Entity, "sticker");


uptr<Entity> create_sticker(World &world, Core &core)
{
  return uptr<Entity>(new Sticker(world, core));
}

}
}
