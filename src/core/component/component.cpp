#include "component.h"
#include <cassert>
#include "core/world/entity.h"
#include "core/world/world.h"

namespace atom {

Component::Component(Entity &entity)
  : my_entity(entity)
{
  my_entity.register_component(this);
}

Component::~Component()
{

}

void Component::attach()
{
  on_attach();
}

void Component::detach()
{
  on_detach();
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

}
