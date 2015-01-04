#pragma once

#include <core/frame.h>

namespace atom {

class GameFrame : public Frame {
  sptr<World> my_world;

public:
  explicit GameFrame(Core &core, const String &level_name);

  ~GameFrame();

  void input() override;

  void update() override;

  void draw() override;
};

}
