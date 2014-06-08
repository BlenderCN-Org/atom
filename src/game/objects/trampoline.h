#pragma once

#include <core/world/entity.h>
#include <core/component/pixmap_component.h>
#include <core/component/rigid_body_component.h>
#include <core/component/fixture_component.h>

namespace atom {
namespace game {

class Trampoline : public Entity {
  META_DECLARE_CLASS;
public:
  static uptr<Entity> create(World &world, Core &core);

  uptr<Entity> clone(World &world) const override;

  void on_init() override;

  void on_update() override;

private:
  Trampoline(World &world, Core &core);

  static f32 WIDTH;
  static f32 HEIGHT;

private:
  PixmapComponent     my_pixmap;
  RigidBodyComponent  my_body;
  BoxFixtureComponent my_fixture;
  Vec2f               my_impulse;
  f32                 my_countdown;
  f32                 my_counter;
};

}
}
