#pragma once

#include <core/world/entity.h>
#include <core/world/world.h>
#include <core/component/pixmap_component.h>
#include <core/component/rigid_body_component.h>
#include <core/component/fixture_component.h>

namespace atom {
namespace game {

class MovingPlatform : public Entity {
//  META_DECLARE_CLASS;
public:
  static uptr<Entity> create(World &world, Core &core);

  MovingPlatform(World &world, Core &core);


  void on_init() override;

  void on_update() override;

  uptr<Entity> clone(World &world) const override;


  static const f32 WIDTH;
  static const f32 HEIGHT;

private:
  PixmapComponent     my_pixmap;
  RigidBodyComponent  my_body;
  BoxFixtureComponent my_fixture;
};

}
}
