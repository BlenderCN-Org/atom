#include <core/constants.h>
#include <core/log.h>
#include <core/frame.h>
#include <core/system/game_api.h>
#include <core/component/mesh_component.h>
#include <core/component/render_component.h>
#include <core/system/resource_service.h>
#include <core/component/material_component.h>
#include <core/component/script_component.h>
#include <core/component/skeleton_component.h>
#include <core/component/collider_component.h>
#include <core/component/rigid_body_component.h>
#include <core/component/model_component.h>
#include "monster.h"

namespace atom {
namespace {

class SkeletonBodyScript : public ScriptComponent {
  Slot<ModelComponent>    my_model;
  Slot<SkeletonComponent> my_skeleton;

  uptr<Component> clone() const override
  {
    return uptr<Component>(new SkeletonBodyScript());
  }

public:
  SkeletonBodyScript()
    : my_model(this)
    , my_skeleton(this)
  {

  }

  void update() override
  {

  }
};


class AnimalScript : public ScriptComponent {
  u32 my_tick;
  Slot<SkeletonComponent> my_skeleton;

  uptr<Component> clone() const override
  {
    return uptr<Component>(new AnimalScript());
  }

public:
  AnimalScript()
    : my_tick(0)
    , my_skeleton(this)
  {

  }

  void update() override
  {
    ++my_tick;
    f32 angle1 = my_tick / 10.0f;
    f32 angle2 = sin(angle1) / 3;

    AxisAnglef aa;
    aa.axis = Vec3f(0, 0, 1);
    aa.angle = angle1;
//    my_skeleton->find_bone("bmain")->transform = Quatf::from_axis_angle(Vec3f::axis_z(), angle1);
    my_skeleton->find_bone("bleg_rr")->transform = Quatf::from_axis_angle(Vec3f::x_axis(), angle2);
    my_skeleton->find_bone("bleg_rl")->transform = Quatf::from_axis_angle(Vec3f::x_axis(), -angle2);
    my_skeleton->find_bone("bleg_fr")->transform = Quatf::from_axis_angle(Vec3f::x_axis(), angle2);
    my_skeleton->find_bone("bleg_fl")->transform = Quatf::from_axis_angle(Vec3f::x_axis(), -angle2);

    my_skeleton->recalculate_skeleton();
  }
};



Frame* create_first_frame(Core &core)
{
  return nullptr;
//  return new game::DemoFrame(core);
  //return new game::MainMenuFrame(core, std::make_shared<game::DemoFrame>(core));
}


class MySimpleUpdate : public ScriptComponent {
  uptr<Component> clone() const override
  {
    return uptr<Component>(new MySimpleUpdate());
  }

public:
  void update() override
  {
    Mat4f t = entity().transform() * Mat4f::translation(0, 0, 0.1f);
    entity().set_transform(t);
  }
};



uptr<Entity> create_test_object(World &world, Core &core)
{
  uptr<Entity> entity(new Entity(world, core));
  // suzanne
  uptr<ModelComponent> model(new ModelComponent("animal"));
  uptr<MaterialComponent> material(new MaterialComponent("animal"));
  uptr<MeshComponent> mesh(new MeshComponent());
  uptr<SkeletonComponent> skeleton(new SkeletonComponent());
  uptr<RenderComponent> render(new RenderComponent());
  uptr<ScriptComponent> script(new AnimalScript());
  entity->add_component(std::move(model));
  entity->add_component(std::move(material));
  entity->add_component(std::move(mesh));
  entity->add_component(std::move(skeleton));
  entity->add_component(std::move(render));
  entity->add_component(std::move(script));
  return entity;
}

uptr<Entity> create_monster(World &world, Core &core)
{
  uptr<Entity> entity(new Entity(world, core));
  // suzanne
  uptr<ModelComponent> model(new ModelComponent("monster"));
  uptr<MaterialComponent> material(new MaterialComponent("animal"));
  uptr<MeshComponent> mesh(new MeshComponent());
  uptr<SkeletonComponent> skeleton(new SkeletonComponent());
  uptr<RenderComponent> render(new RenderComponent());
  uptr<ScriptComponent> script(new MonsterScript());
  entity->add_component(std::move(model));
  entity->add_component(std::move(material));
  entity->add_component(std::move(mesh));
  entity->add_component(std::move(skeleton));
  entity->add_component(std::move(script));
  entity->add_component(std::move(render));
  return entity;
}

uptr<Entity> create_ground(World &world, Core &core)
{
  uptr<Entity> entity(new Entity(world, core));
  uptr<PlaneColliderComponent> collider(new PlaneColliderComponent(Vec4f(0, 0, 1, 0)));
  uptr<RigidBodyComponent> rigid_body(new RigidBodyComponent());
  rigid_body->set_type(RigidBodyType::STATIC);
  rigid_body->set_mass(0);
  entity->add_component(std::move(collider));
  entity->add_component(std::move(rigid_body));
  return entity;
}

uptr<Entity> create_box(World &world, Core &core)
{
  uptr<Entity> entity(new Entity(world, core));
  uptr<BoxColliderComponent> collider(new BoxColliderComponent(Vec3f(1, 1, 1)));
  uptr<RigidBodyComponent> rigid_body(new RigidBodyComponent());
  entity->add_component(std::move(collider));
  entity->add_component(std::move(rigid_body));
  return entity;
}

uptr<Entity> create_suzanne(World &world, Core &core)
{
  uptr<Entity> entity(new Entity(world, core));
  // suzanne
  uptr<ModelComponent> model(new ModelComponent("suzzane"));
  uptr<MaterialComponent> material(new MaterialComponent("test2"));
  uptr<MeshComponent> mesh(new MeshComponent());
  uptr<RenderComponent> render(new RenderComponent());
  entity->add_component(std::move(model));
  entity->add_component(std::move(material));
  entity->add_component(std::move(mesh));
  entity->add_component(std::move(render));
  return entity;
}

std::vector<EntityCreator> create_object_creators(Core &)
{
  std::vector<EntityCreator> creators;

  creators.push_back(EntityCreator("TestObject", create_test_object));
  creators.push_back(EntityCreator("Suzanne", create_suzanne));
  creators.push_back(EntityCreator("Monster", create_monster));
  creators.push_back(EntityCreator("Ground", create_ground));
  creators.push_back(EntityCreator("Box", create_box));
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

GameAPI game_api = {
  create_first_frame,
  create_object_creators
};

}
}

extern "C" {

const atom::EntryPoint* entry_point()
{
  return &atom::game_api;
}

}
