#include "box.h"
#include <core/system/resource_service.h>
#include <core/processor/physics_processor.h>
#include <core/processor/video_processor.h>
#include <core/world/world.h>
#include <core/component/pixmap_component.h>
#include <core/component/rigid_body_component.h>
#include <core/component/fixture_component.h>
#include "game/constants.h"

namespace atom {
namespace game {

class Box : public Entity {
  META_DECLARE_CLASS;

  f32                 my_density;
  RigidBodyComponent  my_body;
  BoxFixtureComponent my_fixture;
  PixmapComponent     my_pixmap;
public:
  explicit Box(World &world, Core &core)
    : Entity(world, core)
    , my_density(2)
    , my_body(RigidBodyType::DYNAMIC, *this)
    , my_fixture(Vec2f(1, 1), Vec2f(0, 0), my_body, *this)
    , my_pixmap(*this)
  {
    META_INIT();
    my_fixture.set_filter(CATEGORY_GROUND, MASK_GROUND);
    my_pixmap.set_material("box");
    my_pixmap.set_auto_transform(true);
    my_body.set_platform(true);
  }

  void on_init() override
  {
    my_fixture.set_density(my_density);
    my_body.set_position(position());
  }

  void on_update() override
  {
    set_position(my_body.position());
  }

  uptr<Entity> clone(World &world) const override
  {
    uptr<Box> duplicate(new Box(world, core()));
    copy_field_values(*this, *duplicate);
    return std::move(duplicate);
  }

};

META_DEFINE_FIELDS(Box) {
  FIELD(Box, my_density, "density")
};

META_DEFINE_CLASS(Box, Entity, "Box");

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

class StaticBox : public Entity {
  META_DECLARE_CLASS;
public:
  StaticBox(World &world, Core &core)
    : Entity(world, core)
    , my_size(1, 1)
    , my_pixmap(*this)
    , my_body(RigidBodyType::STATIC, *this)
    , my_fixture(Vec2f(1, 1), Vec2f(0, 0), my_body, *this)
  {
    META_INIT();
    my_fixture.set_filter(CATEGORY_GROUND, MASK_GROUND);
    my_body.set_platform(true);
    my_pixmap.set_material("static_box");
    my_pixmap.set_auto_transform(true);
  }

  void on_init() override
  {
    set_size(my_size.x, my_size.y);
    my_body.set_position(position());
    my_fixture.set_size(my_size.x, my_size.y - 0.2);
    my_pixmap.set_size(my_size);
  }

  uptr<Entity> clone(World &world) const override
  {
    uptr<StaticBox> duplicate(new StaticBox(world, core()));
    copy_field_values(*this, *duplicate);
    return std::move(duplicate);
  }

private:
  Vec2f               my_size;
  PixmapComponent     my_pixmap;
  RigidBodyComponent  my_body;
  BoxFixtureComponent my_fixture;
};

META_DEFINE_FIELDS(StaticBox) {
  FIELD(StaticBox, my_size, "size")
};

META_DEFINE_CLASS(StaticBox, Entity, "StaticBox");

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

class SimpleStaticPlatform : public Entity {
  META_DECLARE_CLASS;
public:
  SimpleStaticPlatform(World &world, Core &core)
    : Entity(world, core)
    , my_length(1)
    , my_pixmap(*this)
    , my_body(RigidBodyType::STATIC, *this)
    , my_fixture(Vec2f(1, 1), Vec2f(0, 0), my_body, *this)
  {
    META_INIT();
    my_fixture.set_filter(CATEGORY_GROUND, MASK_GROUND);
    my_body.set_platform(true);
    my_pixmap.set_material("platform");
    my_pixmap.set_auto_transform(true);
  }

  void on_init() override
  {
    set_size(my_length, HEIGHT);
    my_body.set_position(position());
    my_fixture.set_size(my_length, HEIGHT - 0.2);
    my_pixmap.set_uv(Vec2f(0, 0), Vec2f(my_length / 2, 1));
    my_pixmap.set_size(Vec2f(my_length, HEIGHT));
  }

  uptr<Entity> clone(World &world) const override
  {
    uptr<SimpleStaticPlatform> duplicate(new SimpleStaticPlatform(world, core()));
    copy_field_values(*this, *duplicate);
    return std::move(duplicate);
  }

  static const f32 HEIGHT;

private:
  f32                 my_length;
  PixmapComponent     my_pixmap;
  RigidBodyComponent  my_body;
  BoxFixtureComponent my_fixture;
};

const f32 SimpleStaticPlatform::HEIGHT = 1;

META_DEFINE_FIELDS(SimpleStaticPlatform) {
  FIELD(SimpleStaticPlatform, my_length, "length")
};

META_DEFINE_CLASS(SimpleStaticPlatform, Entity, "SimpleStaticPlatform");

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

uptr<Entity> create_box(World &world, Core &core)
{
  return uptr<Entity>(new Box(world, core));
}

uptr<Entity> create_static_box(World &world, Core &core)
{
  return uptr<Entity>(new StaticBox(world, core));
}

uptr<Entity> create_simple_static_platform(World &world, Core &core)
{
  return uptr<Entity>(new SimpleStaticPlatform(world, core));
}

}
}
