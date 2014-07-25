#pragma once

#include "component.h"

namespace atom {

class MeshComponent : public Component {
  String my_mesh_name;
  MeshResourcePtr my_mesh;

  uptr<Component> clone() const override;

  void activate() override;

  void deactivate() override;

public:
  MeshComponent(const String &mesh);
  ~MeshComponent();



  MeshResourcePtr mesh() const;
};

MAP_COMPONENT_TYPE(MeshComponent, MESH)

}
