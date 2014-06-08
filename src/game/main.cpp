#include <core/constants.h>
#include <core/log.h>
#include <core/frame.h>
#include <core/system/game_api.h>
#include "game/objects/player.h"
#include "game/objects/box.h"
#include "game/objects/sticker.h"
#include "game/objects/automatic_camera.h"
#include "game/objects/moving_platform.h"
#include "game/objects/falling_platform.h"
#include "game/objects/trampoline.h"

using namespace atom;

namespace atom {
namespace {

Frame* create_first_frame(Core &core)
{
  return nullptr;
//  return new game::DemoFrame(core);
  //return new game::MainMenuFrame(core, std::make_shared<game::DemoFrame>(core));
}

std::vector<EntityCreator> create_object_creators(Core &)
{
  std::vector<EntityCreator> creators;

  creators.push_back(EntityCreator("Player", game::Player::create));
  creators.push_back(EntityCreator("Box", game::create_box));
  creators.push_back(EntityCreator("Static Box", game::create_static_box));
  creators.push_back(EntityCreator("Platform", game::create_simple_static_platform));
  creators.push_back(EntityCreator("Moveable", game::MovingPlatform::create));
  creators.push_back(EntityCreator("FallingPlatform", game::FallingPlatform::create));
  creators.push_back(EntityCreator("Sticker", game::create_sticker));
  creators.push_back(EntityCreator("Automatic Camera", game::create_automatic_camera));
  creators.push_back(EntityCreator("Trampoline", game::Trampoline::create));
  return creators;
}

//QQQ mozno vytiahnut mimo anonymny namespace
GameAPI game_api = {
  create_first_frame,
  create_object_creators
};

}
}

extern "C" {

const EntryPoint* entry_point()
{
  return &game_api;
}

}
