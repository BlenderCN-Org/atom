#include "material_component.h"
#include "../system/resource_service.h"

namespace atom {

uptr<Component> MaterialComponent::clone() const
{
  uptr<Component> component(new MaterialComponent(my_material_name));
  return component;
}

void MaterialComponent::activate()
{
  my_material = entity().core().resource_service().get_material(my_material_name);
  log::info("Material %p", my_material.get());
}

void MaterialComponent::deactivate()
{
  my_material = nullptr;
}

MaterialComponent::MaterialComponent(const String &name)
  : Component(ComponentType::MATERIAL)
  , my_material_name(name)
{
  assert(!name.empty());
}

MaterialResourcePtr MaterialComponent::material() const
{
  return my_material;
}


}
