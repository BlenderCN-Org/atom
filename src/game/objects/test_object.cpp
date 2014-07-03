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
  , my_mesh(*this, "compound")
  , my_render(*this, my_mesh, core.resource_service().get_material("test2"))
{
}

TestObject::~TestObject()
{
}

uptr<Entity> TestObject::clone(World &world) const
{
  return uptr<Entity>(new TestObject(world, core()));
}

void TestObject::on_welcome()
{
}

void TestObject::on_goodbye()
{
}

void TestObject::on_init()
{
}

void TestObject::on_update()
{
}

}
