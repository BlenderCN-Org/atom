#include "component.h"
#include <cassert>
#include "../world/entity.h"
#include "../world/world.h"

namespace atom {

META_CLASS(Component,
  FIELD(my_type, "type"),
  FIELD(my_name, "name")
)

Component::Component(ComponentType type)
  : my_type(type)
  , my_entity(nullptr)
{
  META_INIT();
}

Component::~Component()
{

}

uptr<Component> Component::duplicate() const
{
  uptr<Component> component = clone();

  // copy properties
  copy_field_values(*this, *component);
  
  return component;
}

void Component::attach(Entity &entity)
{
  my_entity = &entity;
  // first activate slots
  for (GenericSlot *slot : my_slots) {
    slot->activate(entity);
  }
  init();
  // then activate entity
  activate();
}

void Component::detach()
{
  deactivate();
  terminate();
  my_entity = nullptr;
}

Entity& Component::entity() const
{
  assert(my_entity != nullptr && "Called before attach()");
  return *my_entity;
}

World& Component::world() const
{
  return entity().world();
}

Core& Component::core() const
{
  return entity().core();
}

WorldProcessorsRef Component::processors() const
{
  return world().processors();
}

ComponentType Component::type() const
{
  return my_type;
}

const String& Component::name() const
{
  return my_name;
}

void Component::set_name(const String &name)
{
  my_name = name;
}

void Component::register_slot(GenericSlot *slot)
{
  assert(slot != nullptr);
  my_slots.push_back(slot);
}

void GenericSlot::activate(Entity &entity)
{
  my_component = entity.find_component(my_type, my_name);
}

Component* GenericSlot::get() const
{
  return my_component;
}

bool GenericSlot::is_null() const
{
  return my_component == nullptr;
}

}
