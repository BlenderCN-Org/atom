#include "component.h"
#include <cassert>
#include "core/world/entity.h"
#include "core/world/world.h"

namespace atom {

Component::Component(ComponentType type, Entity &entity)
  : my_type(type)
  , my_entity(entity)
{
  my_entity.register_component(this);
}

Component::~Component()
{
  
}

Entity& Component::entity() const
{
  return my_entity;
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
