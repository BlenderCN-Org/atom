#include "geometry_component.h"
#include "../processor/geometry_processor.h"
#include "../world/world.h"

namespace atom {

META_CLASS(GeometryComponent,
  FIELD(my_is_dynamic, "dynamic")
)

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
  , my_model(this)
  , my_skeleton(this)
{
  META_INIT();
}

const Model* GeometryComponent::model() const
{
  if (my_model.is_null()) {
    return nullptr;
  }
  
  ModelResourcePtr resource = my_model->get_model();
  return resource != nullptr ? &resource->model() : nullptr;
}

const SkeletonComponent* GeometryComponent::skeleton() const
{
  return my_skeleton.get_component();
}

}
