#include "entity.h"
#include <algorithm>
#include "../video/mesh.h"
#include "../video/uniforms.h"
#include "../component/component.h"

namespace atom {

META_DEFINE_FIELDS(Entity) {
  FIELD(Entity, my_id, "id"),
  FIELD(Entity, my_class, "class"),
  FIELD(Entity, my_transform, "transform")
};

META_DEFINE_ROOT_CLASS(Entity, "Entity");

Entity::Entity(World &world, Core &core, f32 width, f32 height)
  : my_world(world)
  , my_core(core)
{
  META_INIT();
  init(width, height);
}

Entity::~Entity()
{
}

uptr<Entity> Entity::clone(World &world) const
{
  uptr<Entity> entity(new Entity(world, core()));

  for (const uptr<Component> &component : my_components) {
    uptr<Component> duplicate = component->duplicate();
    entity->add_component(std::move(duplicate));
  }

  return entity;
}

void Entity::activate()
{
  for (const uptr<Component> &component : my_components) {
    component->attach(*this);
  }
}

void Entity::deactivate()
{
  for (const uptr<Component> &component : my_components) {
    component->detach();
  }
}

void Entity::add_component(uptr<Component> &&component)
{
  assert(component != nullptr);
  my_components.push_back(std::move(component));
}

const String& Entity::id() const
{
  return my_id;
}

void Entity::set_id(const String &id)
{
  my_id = id;
}

void Entity::set_size(f32 width, f32 height)
{
  my_width = width;
  my_height = height;
}

const Mat4f& Entity::transform() const
{
  return my_transform;
}

void Entity::set_transform(const Mat4f &transform)
{
  my_transform = transform;
}

void Entity::update_bounding_box()
{
//  my_bounding_box = BoundingBox::from_params(my_position, my_width, my_height);
}

const String &Entity::class_name() const
{
  return my_class;
}

void Entity::set_class_name(const String &class_name)
{
  my_class = class_name;
}

World& Entity::world() const
{
  return my_world;
}

BoundingBox Entity::bounding_box() const
{
  return my_bounding_box;
}

bool Entity::is_live() const
{
  return my_state == State::RUNNING;
}

Core& Entity::core() const
{
  return my_core;
}

Component* Entity::find_component(const String &name)
{
  auto found = std::find_if(my_components.begin(), my_components.end(),
    [name](const uptr<Component> &component) { return component->name() == name; });
  return found != my_components.end() ? found->get() : nullptr;
}

Component* Entity::find_component(ComponentType type)
{
  auto found = std::find_if(my_components.begin(), my_components.end(),
    [type](const uptr<Component> &component) { return component->type() == type; });
  return found != my_components.end() ? found->get() : nullptr;
}

Component* Entity::find_component(ComponentType type, const String &name)
{
  auto found = std::find_if(my_components.begin(), my_components.end(),
    [type, &name](const uptr<Component> &component)
    { return component->type() == type && (name.empty() ? true : component->name() == name); });
  return found != my_components.end() ? found->get() : nullptr;
}

void Entity::init(f32 width, f32 height, const Vec3f &position, f32 rotation)
{
  my_state = State::NEW;
  my_width = width;
  my_height = height;
  my_class = "Base class";
  my_transform = Mat4f::identity();
  META_INIT();
}

}
