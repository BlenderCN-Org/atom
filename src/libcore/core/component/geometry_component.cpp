#include "geometry_component.h"
#include "../processor/geometry_processor.h"

namespace atom {

void GeometryComponent::init()
{
  processors().geometry.register_component(this);
}

void GeometryComponent::terminate()
{
  processors().geometry.unregister_component(this);
}

uptr<Component> GeometryComponent::clone() const
{
  return uptr<Component>(new GeometryComponent());
}

GeometryComponent::GeometryComponent()
  : NullComponent(ComponentType::GEOMETRY)
{
  
}

}
