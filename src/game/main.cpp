#include <core/constants.h>
#include <core/log.h>
#include <core/frame.h>
#include <core/system/game_api.h>
#include <core/component/mesh_component.h>
#include <core/component/render_component.h>
#include <core/system/resource_service.h>

using namespace atom;

namespace atom {
namespace {

Frame* create_first_frame(Core &core)
{
  return nullptr;
//  return new game::DemoFrame(core);
  //return new game::MainMenuFrame(core, std::make_shared<game::DemoFrame>(core));
}

uptr<Entity> create_test_object(World &world, Core &core)
{
  uptr<Entity> entity(new Entity(world, core));
  // suzanne
  uptr<MeshComponent> mesh(new MeshComponent("suzanne"));
  uptr<RenderComponent> render(new RenderComponent(*mesh, core.resource_service().get_material("test2")));
  entity->add_component(std::move(mesh));
  entity->add_component(std::move(render));
  return entity;
}

std::vector<EntityCreator> create_object_creators(Core &)
{
  std::vector<EntityCreator> creators;

  creators.push_back(EntityCreator("TestObject", create_test_object));
//  creators.push_back(EntityCreator("Box", game::create_box));
//  creators.push_back(EntityCreator("Static Box", game::create_static_box));
//  creators.push_back(EntityCreator("Platform", game::create_simple_static_platform));
//  creators.push_back(EntityCreator("Moveable", game::MovingPlatform::create));
//  creators.push_back(EntityCreator("FallingPlatform", game::FallingPlatform::create));
//  creators.push_back(EntityCreator("Sticker", game::create_sticker));
//  creators.push_back(EntityCreator("Automatic Camera", game::create_automatic_camera));
//  creators.push_back(EntityCreator("Trampoline", game::Trampoline::create));
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
