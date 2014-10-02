#include "script_component.h"
#include "../world/world.h"
#include "../processor/script_processor.h"

namespace atom {

void ScriptComponent::init()
{
  on_init();
}

void ScriptComponent::activate()
{
  processors().script.register_script(this);
  on_activate();
}

void ScriptComponent::deactivate()
{
  processors().script.unregister_script(this);
  on_deactivate();
}

void ScriptComponent::terminate()
{
  on_terminate();
}

ScriptComponent::ScriptComponent()
  : NullComponent(ComponentType::SCRIPT)
{
}

void ScriptComponent::update()
{
  on_update();
}

}
