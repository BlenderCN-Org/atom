#pragma once

#include "component.h"

namespace atom {

class ModelComponent : public NullComponent {
  ModelResourcePtr my_model;

  void activate() override;

  void deactivate() override;

public:
  ModelComponent();
  ~ModelComponent();

  void set_model(const ModelResourcePtr& model);

  ModelResourcePtr get_model() const;

  META_SUB_CLASS(NullComponent);
};

MAP_COMPONENT_TYPE(ModelComponent, MODEL)

}
