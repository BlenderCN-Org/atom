#include "entity.h"
#include <algorithm>
#include "../video/mesh.h"
#include "../video/uniforms.h"
#include "../component/component.h"

namespace atom {

META_CLASS(Entity,
  FIELD(my_id, "id"),
  FIELD(my_class, "class"),
  FIELD(my_transform, "transform")
)

Entity::Entity(World &world, Core &core)
  : my_world(world)
  , my_core(core)
  , my_bounding_box(-1, 1, -1, 1, -1, 1)
{
  META_INIT();
  init();
}

Entity::~Entity()
{
}

uptr<Entity> Entity::clone(World &world) const
{
  uptr<Entity> entity(new Entity(world, core()));
  entity->set_bounding_box(my_bounding_box);

  for (const uptr<Component> &component : my_components) {
    uptr<Component> duplicate = component->duplicate();
    entity->set_transform(my_transform);
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

const Mat4f& Entity::transform() const
{
  return my_transform;
}

void Entity::set_transform(const Mat4f &transform)
{
  my_transform = transform;
  update_aabb();
}

const BoundingBox& Entity::bounding_box() const
{
  return my_bounding_box;
}

void Entity::set_bounding_box(const BoundingBox &box)
{
  my_bounding_box = box;
  update_aabb();
}

const BoundingBox& Entity::aabb() const
{
  return my_aabb;
}

const String& Entity::class_name() const
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

void Entity::init(const Vec3f &position, f32 rotation)
{
  my_state = State::NEW;
  my_class = "Base class";
  my_transform = Mat4f();
}

void Entity::update_aabb()
{
  my_aabb = BoundingBox();
  const BoundingBox &box = my_bounding_box;
  Vec3f xmin_ymin_zmin = Vec3f(box.xmin, box.ymin, box.zmin);
  Vec3f xmax_ymin_zmin = Vec3f(box.xmax, box.ymin, box.zmin);
  Vec3f xmin_ymax_zmin = Vec3f(box.xmin, box.ymax, box.zmin);
  Vec3f xmax_ymax_zmin = Vec3f(box.xmax, box.ymax, box.zmin);
  Vec3f xmin_ymin_zmax = Vec3f(box.xmin, box.ymin, box.zmax);
  Vec3f xmax_ymin_zmax = Vec3f(box.xmax, box.ymin, box.zmax);
  Vec3f xmin_ymax_zmax = Vec3f(box.xmin, box.ymax, box.zmax);
  Vec3f xmax_ymax_zmax = Vec3f(box.xmax, box.ymax, box.zmax);

  my_aabb.extend(transform_point(my_transform, xmin_ymin_zmin));
  my_aabb.extend(transform_point(my_transform, xmax_ymin_zmin));
  my_aabb.extend(transform_point(my_transform, xmin_ymax_zmin));
  my_aabb.extend(transform_point(my_transform, xmax_ymax_zmin));
  my_aabb.extend(transform_point(my_transform, xmin_ymin_zmax));
  my_aabb.extend(transform_point(my_transform, xmax_ymin_zmax));
  my_aabb.extend(transform_point(my_transform, xmin_ymax_zmax));
  my_aabb.extend(transform_point(my_transform, xmax_ymax_zmax));
}

}
