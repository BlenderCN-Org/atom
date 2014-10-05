#include "player.h"
#include <core/world/entity.h>
#include <core/component/script_component.h>
#include <core/component/model_component.h>
#include <core/component/mesh_component.h>
#include <core/component/render_component.h>

namespace atom {

class PlayerScript : public ScriptComponent {
};

uptr<Entity> create_player(World &world, Core &core)
{
  uptr<Entity> entity(new Entity(world, core));
  uptr<ModelComponent> model(new ModelComponent());
  model->set_model_name("player");
  entity->add_component(std::move(model));
  uptr<MaterialComponent> material(new MaterialComponent());
  material->set_material_name("player");
  entity->add_component(std::move(material));
  
  entity->add_component(uptr<Component>(new MeshComponent()));
  entity->add_component(uptr<Component>(new RenderComponent()));
  return entity;
}

}
