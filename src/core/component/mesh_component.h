#pragma once

#include "component.h"

namespace atom {

class MeshComponent : public Component {
  String my_mesh_name;
  MeshResourcePtr my_mesh;
  
public:
  MeshComponent(const String &mesh);
  ~MeshComponent();
  
  void attach() override;
  
  void detach() override;

  uptr<Component> clone() const override;
  
  MeshResourcePtr mesh() const;
};

MAP_COMPONENT_TYPE(MeshComponent, MESH)

}
