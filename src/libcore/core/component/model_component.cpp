#include "model_component.h"
#include "../system/core.h"
#include "../system/resource_service.h"

namespace atom {

void ModelComponent::activate()
{
  my_model = core().resource_service().get_model(my_model_name);

  if (my_model == nullptr) {
    log::warning("Can't find model \"%s\"", my_model_name.c_str());
  }
}

void ModelComponent::deactivate()
{
  my_model.reset();
}

uptr<Component> ModelComponent::clone() const
{
  return uptr<Component>(new ModelComponent(my_model_name));
}

ModelComponent::ModelComponent(const String &name)
  : NullComponent(ComponentType::MODEL)
  , my_model_name(name)
{

}

ModelComponent::~ModelComponent()
{
  // empty
}

ModelResourcePtr ModelComponent::get_model() const
{
  return my_model;
}

const String &ModelComponent::get_model_name() const
{
  return my_model_name;
}

}
