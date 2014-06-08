#pragma once

#include <core/world/entity.h>
#include <core/math/camera.h>
#include <core/video/mesh_tree_node.h>
#include <core/component/rigid_body_component.h>
#include <core/component/fixture_component.h>
#include <core/component/joint_component.h>
#include <core/component/pixmap_component.h>

namespace atom {
namespace game {

class Player : public Entity {
public:
  static uptr<Entity> create(World &world, Core &core);

  Player(World &world, Core &core);

  uptr<Entity> clone(World &world) const override;

  void on_init() override;

  void on_update() override;

private:
  enum class State {
    WALKING,
    IDLE,
    JUMPING,
    ROPING
  };

  enum class Direction {
    LEFT,
    RIGHT
  };

  void switch_state(State state);

private:
  State               my_state;
  RigidBodyComponent  my_body;
  BoxFixtureComponent my_body_collider;
  BoxFixtureComponent my_ground_collider;
  PixmapComponent     my_pixmap;
  PixmapComponent     my_pointer;
  RopeJointComponent  my_rope;
  u32                 my_walking_track_id;
  Direction           my_direction;
};

}
}
