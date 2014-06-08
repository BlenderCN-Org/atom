#pragma once

#include "core/component/component.h"
#include "core/component/rigid_body_component.h"
#include "core/component/fixture_component.h"

namespace atom {

class SensorComponent : public Component {
public:
  explicit SensorComponent(Entity &entity);

  void set_filter(u16 category, u16 mask);

  void on_attach() override;

  void on_detach() override;

  void update();

  bool is_triggered() const;

private:
  RigidBodyComponent  my_body;
  BoxFixtureComponent my_fixture;
};

}
