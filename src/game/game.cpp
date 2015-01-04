#include <core/constants.h>
#include <core/log.h>
#include <core/frame.h>
#include <core/game_entry.h>
#include <core/mesh_component.h>
#include <core/render_component.h>
#include <core/resource_service.h>
#include <core/material_component.h>
#include <core/script_component.h>
#include <core/skeleton_component.h>
#include <core/collider_component.h>
#include <core/rigid_body_component.h>
#include <core/geometry_component.h>
#include <core/model_component.h>
#include <core/model.h>
#include <core/mesh.h>
#include "game_frame.h"
#include "monster.h"
#include "player.h"
#include "game.h"

namespace atom {
namespace {

class SkeletonBodyScript : public ScriptComponent {
  Slot<ModelComponent>    my_model;
  Slot<SkeletonComponent> my_skeleton;
  Slot<MeshComponent>     my_mesh;
  Slot<RenderComponent>   my_render;
  std::vector<Vec3f>      my_vertices;
  std::vector<Vec3f>      my_last_vertices;
  MeshResourcePtr         my_mesh_resource;
  bool                    my_is_initialized;
  VideoBuffer            *my_mesh_vertices;

  uptr<Component> clone() const override
  {
    return uptr<Component>(new SkeletonBodyScript());
  }

  void on_update() override
  {
    const Model &model = my_model->get_model()->model();

    Slice<f32> vertex_stream = model.find_stream<f32>(MODEL_VERTEX);
    Slice<f32> bweight_stream = model.find_stream<f32>(MODEL_BONE_WEIGHT);
    Slice<u32> bindex_stream = model.find_stream<u32>(MODEL_BONE_INDEX);

    if (!my_is_initialized) {
      Slice<u32> index_stream = model.find_stream<u32>(MODEL_INDEX);
      my_is_initialized = true;
      Mesh *mesh = my_mesh_resource->data();

      uptr<VideoBuffer> mesh_indices(new VideoBuffer(core().video_service(), VideoBufferUsage::STATIC_DRAW));
      uptr<VideoBuffer> mesh_vertices(new VideoBuffer(core().video_service(), VideoBufferUsage::DYNAMIC_DRAW));

      mesh_indices->set_data(index_stream);

      mesh->vertex = std::move(mesh_vertices);
      mesh->surface = std::move(mesh_indices);
      my_mesh_vertices = mesh->vertex.get();
      my_mesh_vertices->set_data(vertex_stream);
      my_mesh->set_mesh(my_mesh_resource);
      my_render->set_enabled(true);
    }


    u32 vertex_count = vertex_stream.raw_size() / sizeof(Vec3f);
    u32 bweight_count = bweight_stream.raw_size() / sizeof(Vec4f);
    u32 bindex_count = bindex_stream.raw_size() / sizeof(Vec4u8);

    if (vertex_count != bweight_count || vertex_count != bindex_count) {
      log::error("Corrupted model data arrays");
      return;
    }

    const Vec3f *vertices = reinterpret_cast<const Vec3f *>(vertex_stream.data());
    const Vec4f *bone_weight = reinterpret_cast<const Vec4f *>(bweight_stream.data());
    const Vec4u8 *bone_index = reinterpret_cast<const Vec4u8 *>(bindex_stream.data());

    my_last_vertices.swap(my_vertices);
    my_vertices.clear();
    my_vertices.reserve(vertex_count);

    my_skeleton->recalculate_skeleton();

    const Slice<Mat4f> transforms = my_skeleton->get_transforms();

    for (u32 i = 0; i < vertex_count; ++i) {
      const Vec4f v = Vec4f(vertices[i], 1);
      const Vec4f &weight = bone_weight[i];
      const Vec4u8 index = bone_index[i];

      const Mat4f &m0 = transforms[index[0]];
      const Mat4f &m1 = transforms[index[1]];
      const Mat4f &m2 = transforms[index[2]];
      const Mat4f &m3 = transforms[index[3]];

      const Vec3f v0 = (m0 * v * weight[0]).xyz();
      const Vec3f v1 = (m1 * v * weight[1]).xyz();
      const Vec3f v2 = (m2 * v * weight[2]).xyz();
      const Vec3f v3 = (m3 * v * weight[3]).xyz();
      my_vertices.push_back(v0 + v1 + v2 + v3);

      my_mesh_vertices->set_data(to_slice(my_vertices));
    }
  }

public:
  SkeletonBodyScript()
    : my_model(this)
    , my_skeleton(this)
    , my_mesh(this)
    , my_render(this)
    , my_is_initialized(false)
    , my_mesh_vertices(nullptr)
  {
    my_mesh_resource.reset(new MeshResource());
    my_mesh_resource->set_data(uptr<Mesh>(new Mesh()));
  }
};


class AnimalScript : public ScriptComponent {
  u32 my_tick;
  Slot<SkeletonComponent> my_skeleton;

  uptr<Component> clone() const override
  {
    return uptr<Component>(new AnimalScript());
  }

  void on_update() override
  {
    ++my_tick;
    f32 angle1 = my_tick / 10.0f;
    f32 angle2 = sin(angle1) / 30;

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

public:
  AnimalScript()
    : my_tick(0)
    , my_skeleton(this)
  {

  }


};



Frame* create_first_frame(Core &core)
{
  return new GameFrame(core, "data/level/terrain_level.lev");
}


uptr<Entity> create_test_object(World &world, Core &core)
{
  uptr<Entity> entity(new Entity(world, core));
  // suzanne
  uptr<ModelComponent> model(new ModelComponent());
  model->set_model_name("animal");
  uptr<MaterialComponent> material(new MaterialComponent());
  material->set_material_name("animal");
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
  uptr<ModelComponent> model(new ModelComponent());
  model->set_model_name("monster");
  uptr<MaterialComponent> material(new MaterialComponent());
  material->set_material_name("flat");
  uptr<MeshComponent> mesh(new MeshComponent());
  uptr<SkeletonComponent> skeleton(new SkeletonComponent());
  uptr<GeometryComponent> geometry(new GeometryComponent());
  geometry->set_dynamic(true);
  geometry->set_categories(CollisionMask::ENEMY);
  uptr<RenderComponent> render(new RenderComponent());
  uptr<ScriptComponent> script(new MonsterScript());
  entity->set_bounding_box(BoundingBox(-20, 20, -20, 20, 0, 20));
  entity->add_component(std::move(model));
  entity->add_component(std::move(material));
  entity->add_component(std::move(mesh));
  entity->add_component(std::move(skeleton));
  entity->add_component(std::move(geometry));
  entity->add_component(std::move(script));
  entity->add_component(std::move(render));
  return entity;
}

uptr<Entity> create_manual_monster(World &world, Core &core)
{
  uptr<Entity> entity(new Entity(world, core));
  uptr<ModelComponent> model(new ModelComponent());
  model->set_model_name("monster");
  uptr<MaterialComponent> material(new MaterialComponent());
  material->set_material_name("manual");
  uptr<MeshComponent> mesh(new MeshComponent());
  mesh->set_mode(MeshComponentMode::MANUAL);
  uptr<SkeletonComponent> skeleton(new SkeletonComponent());
  uptr<RenderComponent> render(new RenderComponent());
  render->set_enabled(false);
  uptr<ScriptComponent> skeleton_script(new MonsterScript());
  uptr<SkeletonBodyScript> script(new SkeletonBodyScript());
  entity->add_component(std::move(model));
  entity->add_component(std::move(material));
  entity->add_component(std::move(mesh));
  entity->add_component(std::move(skeleton));
  entity->add_component(std::move(script));
  entity->add_component(std::move(skeleton_script));
  entity->add_component(std::move(render));
  return entity;
}


uptr<Entity> create_ground(World &world, Core &core)
{
  uptr<Entity> entity(new Entity(world, core));
  uptr<PlaneColliderComponent> collider(new PlaneColliderComponent());
  collider->set_plane(Vec3f(0, 0, 1), 0);
  uptr<RigidBodyComponent> rigid_body(new RigidBodyComponent());
  rigid_body->set_body_type(RigidBodyType::STATIC);
  rigid_body->set_mass(0);
  entity->add_component(std::move(collider));
  entity->add_component(std::move(rigid_body));
  return entity;
}

uptr<Entity> create_box(World &world, Core &core)
{
  uptr<Entity> entity(new Entity(world, core));
  uptr<BoxColliderComponent> collider(new BoxColliderComponent());
  collider->set_size(Vec3f(1, 1, 1));
  uptr<RigidBodyComponent> rigid_body(new RigidBodyComponent());
  entity->add_component(std::move(collider));
  entity->add_component(std::move(rigid_body));
  return entity;
}

uptr<Entity> create_suzanne(World &world, Core &core)
{
  uptr<Entity> entity(new Entity(world, core));
  // suzanne
  uptr<ModelComponent> model(new ModelComponent());
  model->set_model_name("suzzane");
  uptr<MaterialComponent> material(new MaterialComponent());
  material->set_material_name("flat");
  uptr<MeshComponent> mesh(new MeshComponent());
  uptr<RenderComponent> render(new RenderComponent());
  entity->add_component(std::move(model));
  entity->add_component(std::move(material));
  entity->add_component(std::move(mesh));
  entity->add_component(std::move(render));
  return entity;
}

uptr<Entity> create_flat_terrain(World &world, Core &core)
{
  uptr<Entity> entity(new Entity(world, core));
  uptr<ModelComponent> model(new ModelComponent());
  model->set_model_name("quad_terrain");
  uptr<MaterialComponent> material(new MaterialComponent());
  material->set_material_name("terrain");
  uptr<MeshComponent> mesh(new MeshComponent());
  uptr<RenderComponent> render(new RenderComponent());
  uptr<GeometryComponent> geometry(new GeometryComponent());
  geometry->set_categories(CollisionMask::WORLD);
  entity->add_component(std::move(model));
  entity->add_component(std::move(material));
  entity->add_component(std::move(mesh));
  entity->add_component(std::move(render));
  entity->add_component(std::move(geometry));
  return entity;
}

uptr<Entity> create_bumpy_terrain(World &world, Core &core)
{
  uptr<Entity> entity(new Entity(world, core));
  uptr<ModelComponent> model(new ModelComponent());
  model->set_model_name("bumpy_terrain");
  uptr<MaterialComponent> material(new MaterialComponent());
  material->set_material_name("terrain");
  uptr<MeshComponent> mesh(new MeshComponent());
  uptr<RenderComponent> render(new RenderComponent());
  uptr<GeometryComponent> geometry(new GeometryComponent());
  geometry->set_categories(CollisionMask::WORLD);
  entity->add_component(std::move(model));
  entity->add_component(std::move(material));
  entity->add_component(std::move(mesh));
  entity->add_component(std::move(render));
  entity->add_component(std::move(geometry));
  return entity;
}

std::vector<EntityCreator> create_object_creators(Core &)
{
  std::vector<EntityCreator> creators;

  creators.push_back(EntityCreator("TestObject", create_test_object));
  creators.push_back(EntityCreator("Suzanne", create_suzanne));
  creators.push_back(EntityCreator("Monster", create_monster));
  creators.push_back(EntityCreator("ManualMonster", create_manual_monster));
  creators.push_back(EntityCreator("Ground", create_ground));
  creators.push_back(EntityCreator("Box", create_box));
  creators.push_back(EntityCreator("FlatTerrain", create_flat_terrain));
  creators.push_back(EntityCreator("BumpyTerrain", create_bumpy_terrain));
  creators.push_back(EntityCreator("Player", create_player));
  return creators;
}

GameEntry game_api = {
  create_first_frame,
  create_object_creators
};

}
}

extern "C" {

const atom::GameEntry* game_entry()
{
  return &atom::game_api;
}

}
