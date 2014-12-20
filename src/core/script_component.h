#pragma once

#include "component.h"

namespace atom {

class ScriptComponent : public NullComponent {

  void init() override;
  
  void activate() override;

  void deactivate() override;
  
  void terminate() override;
  
  virtual void on_init()
  {
    
  }
  
  virtual void on_activate()
  {
    
  }
  
  virtual void on_deactivate()
  {
    
  }
  
  virtual void on_terminate()
  {
    
  }
  
  virtual void on_update() = 0;

public:
  ScriptComponent();

  // called only by ScriptProcessor
  void update();
};

MAP_COMPONENT_TYPE(ScriptComponent, SCRIPT)

}
