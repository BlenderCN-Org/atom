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

void Component::set_entity(Entity *entity)
{
  my_entity = entity;
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

}
