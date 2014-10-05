#pragma once

#include "component.h"

namespace atom {

class ModelComponent : public NullComponent {
  String           my_model_name;
  ModelResourcePtr my_model;

  void activate() override;

  void deactivate() override;

  uptr<Component> clone() const override;

public:
  META_DECLARE_CLASS;
  
  ModelComponent();
  ~ModelComponent();
  
  void set_model_name(const String &model_name);

  ModelResourcePtr get_model() const;

  const String& get_model_name() const;
};

MAP_COMPONENT_TYPE(ModelComponent, MODEL)

}
