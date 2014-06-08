#pragma once

#include <core/world/entity.h>
#include <core/world/world.h>
#include <core/component/pixmap_component.h>
#include <core/component/rigid_body_component.h>
#include <core/component/fixture_component.h>

namespace atom {
namespace game {

class FallingPlatform : public Entity {
//  META_DECLARE_CLASS;
public:
  static uptr<Entity> create(World &world, Core &core);

  void on_init() override;

  void on_update() override;

  uptr<Entity> clone(World &world) const override;


  static const f32 WIDTH;
  static const f32 HEIGHT;

private:
  enum class State {
    IDLE,
    FALLING
  };

  FallingPlatform(World &world, Core &core);

  void switch_state(State state);

private:
  State               my_state;
  f32                 my_fall_start;
  PixmapComponent     my_pixmap;
  RigidBodyComponent  my_body;
  BoxFixtureComponent my_fixture;
};


}
}
