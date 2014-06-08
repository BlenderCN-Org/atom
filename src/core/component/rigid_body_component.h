#pragma once

#include <Box2D/Box2D.h>
#include "core/component/component.h"
#include "core/math/vec.h"

namespace atom {

enum class RigidBodyType {
  STATIC,
  DYNAMIC,
  KINEMATIC
};

class RigidBodyComponent : public Component {
public:
  RigidBodyComponent(RigidBodyType type, Entity &entity);

  f32 mass() const;

  void on_attach() override;
  void on_detach() override;

  Vec2f position() const;

  RigidBodyType type() const;

  bool is_entity_update_enabled() const;

  void set_update_entity_position(bool update);

  Vec2f linear_velocity() const;

  void set_linear_velocity(const Vec2f &v);

  void set_angle(f32 angle);

  void set_position(const Vec2f &p);

  void apply_force(const Vec2f &force);

  void apply_impulse(const Vec2f &impulse);

  bool fixed_rotation() const;

  void set_fixed_rotation(bool enable);

  bool is_platform() const;

  void set_platform(bool platform);

  b2Body *b2_body() const;

private:
  RigidBodyType    my_type;
//  f32              my_angle;
  bool             my_update_entity_position;
  bool             my_fixed_rotation;
  bool             my_is_platform;
  b2Body          *my_body;
};

}
