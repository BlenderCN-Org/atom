#include "geometry_component.h"
#include "geometry_processor.h"
#include "world.h"
#include "model_component.h"
#include "skeleton_component.h"

namespace atom {

META_CLASS(GeometryComponent,
  FIELD(my_is_dynamic, "dynamic"),
  FIELD(my_categories, "categories")
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
  , my_is_dynamic(false)
  , my_categories(U32_MAX)
  , my_model(this)
  , my_skeleton(this)
{
  META_INIT();
}

void GeometryComponent::set_categories(u32 mask)
{
  my_categories = mask;
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
