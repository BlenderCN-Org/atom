#include "model_component.h"
#include "core.h"
#include "resource_service.h"
#include "log.h"

namespace atom {

META_CLASS(ModelComponent,
)

void ModelComponent::activate()
{
}

void ModelComponent::deactivate()
{
  my_model.reset();
}

ModelComponent::ModelComponent()
  : NullComponent(ComponentType::MODEL)
{
  META_INIT();
}

ModelComponent::~ModelComponent()
{
  // empty
}

void ModelComponent::set_model(const ModelResourcePtr &model)
{
  my_model = model;
}

ModelResourcePtr ModelComponent::get_model() const
{
  return my_model;
}

}
