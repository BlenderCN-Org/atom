#include "component.h"
#include <cassert>
#include "core/world/entity.h"
#include "core/world/world.h"

namespace atom {

Component::Component(ComponentType type)
  : my_type(type)
  , my_entity(nullptr)
{
}

Component::~Component()
{
  
}

uptr<Component> Component::duplicate() const
{
  uptr<Component> component = clone();
  return component;
}

void Component::attach(Entity &entity)
{
  my_entity = &entity;
  
  for (GenericSlot *slot : my_slots) {
    slot->activate(entity);
  }
  
  activate();
}

void Component::detach()
{
  deactivate();
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

}
