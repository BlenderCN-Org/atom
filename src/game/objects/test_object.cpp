#include "test_object.h"
#include <core/system/core.h>
#include <core/system/resource_service.h>
#include <core/world/world.h>
#include <core/processor/video_processor.h>

namespace atom {

uptr<Entity> TestObject::create(World &world, Core &core)
{
  return uptr<Entity>(new TestObject(world, core));
}

TestObject::TestObject(World &world, Core &core)
  : Entity(world, core)
{
  my_node = std::make_shared<MeshTreeNode>();
}

uptr<Entity> TestObject::clone(World &world) const
{
  return uptr<Entity>(new TestObject(world, core()));
}

void TestObject::on_welcome()
{
  core().resource_service().get_raw_mesh_resource("cube");
  VideoProcessor &vp = world().processors().video;
  vp.mesh_tree()->add_node(my_node);
}

void TestObject::on_goodbye()
{

}

void TestObject::on_init()
{
  ResourceService &rs = core().resource_service();
  my_node->material = rs.get_material_resource("flat");
  my_node->transformations = Mat4f::translation(position().x, position().y, 0);
  my_node->visible = true;
  my_node->mesh_resource = rs.get_mesh_resource("cube");
}

void TestObject::on_update()
{
  my_node->transformations = Mat4f::translation(position().x, position().y, 0);
}

}
