#pragma once

#include "core/corefwd.h"
#include "core/math/vec.h"
#include "core/component/component.h"
#include <Box2D/Box2D.h>

namespace atom {

class FixtureComponent : public Component {
public:
  FixtureComponent(RigidBodyComponent &body, Entity &entity);

  void on_attach() override;
  void on_detach() override;


  bool is_sensor() const;

  void set_sensor(bool sensor);

  u16 category() const;

  void set_filter(u16 category, u16 mask);

  f32 density() const;

  void set_density(f32 density);

  const RigidBodyComponent& rigid_body() const;

  RigidBodyComponent& rigid_body();

  b2Fixture* fixture() const;

  void create_fixture(const b2Shape &shape);
  void destroy_fixture();

  void add_contact(FixtureComponent *component);

  void clear_contacts();

  bool collide() const;

  const std::vector<FixtureComponent *>& contacts() const;

  b2Shape* b2_shape() const;

private:
  b2Fixture                       *my_fixture;
  b2Filter                         my_filter;
  RigidBodyComponent              &my_body;
  std::vector<FixtureComponent *>  my_contacts;
};


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

class BoxFixtureComponent : public FixtureComponent {
public:
  BoxFixtureComponent(const Vec2f &size, const Vec2f &center, RigidBodyComponent &body, Entity &entity);

  void set_size(f32 width, f32 height);
};


}
