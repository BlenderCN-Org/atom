#include "material_component.h"
#include "resource_service.h"

namespace atom {

META_CLASS(MaterialComponent,
  FIELD(my_material_name, "material_name")
)

void MaterialComponent::activate()
{
  my_material = entity().core().resource_service().get_material(my_material_name);
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

void MaterialComponent::set_material_name(const String &name)
{
  my_material_name = name;
}

MaterialResourcePtr MaterialComponent::material() const
{
  return my_material;
}


}
