#include "script_component.h"
#include "core/world/world.h"
#include "core/processor/script_processor.h"

namespace atom {

void ScriptComponent::activate()
{
  processors().script.register_script(this);
}

void ScriptComponent::deactivate()
{
processors().script.unregister_script(this);
}

ScriptComponent::ScriptComponent()
  : Component(ComponentType::SCRIPT)
{
}

}
