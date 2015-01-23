#include "model_component.h"
#include "core.h"
#include "resource_service.h"
#include "log.h"

namespace atom {

META_CLASS(ModelComponent,
  FIELD(my_model_name, "model_name")
)

void ModelComponent::activate()
{
  my_model = core().resource_service().get_model(my_model_name);

  if (my_model == nullptr) {
    log_warning("Can't find model \"%s\"", my_model_name.c_str());
  }
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

void ModelComponent::set_model_name(const String &model_name)
{
  assert(!model_name.empty());
  my_model_name = model_name;
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
