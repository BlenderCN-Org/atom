#include "script_processor.h"
#include <cassert>

namespace atom {

void ScriptProcessor::register_script(SensorComponent *component)
{
  assert(component != nullptr);
//  my_scripts.push_back(component);
}

void ScriptProcessor::unregister_script(SensorComponent *component)
{
  assert(component != nullptr);
//  my_scripts.erase(std::remove(my_scripts.begin(), my_scripts.end(), component), my_scripts.end());
}

void ScriptProcessor::poll()
{
}

void ScriptProcessor::start()
{
//  for (SensorComponent *component : my_scripts) {
//    component->update();
//  }
}

}
