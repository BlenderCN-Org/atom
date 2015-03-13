#include "material_component.h"
#include "resource_service.h"

namespace atom {

META_CLASS(MaterialComponent,
)

void MaterialComponent::activate()
{
}

void MaterialComponent::deactivate()
{
  my_material = nullptr;
}

MaterialComponent::MaterialComponent()
  : NullComponent(ComponentType::MATERIAL)
{
  META_INIT();
}

void MaterialComponent::set_material(const MaterialResourcePtr material)
{
  my_material = material;
}

MaterialResourcePtr MaterialComponent::material() const
{
  return my_material;
}


}
