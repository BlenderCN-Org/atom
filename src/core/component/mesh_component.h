#pragma once

#include "component.h"

namespace atom {

class MeshComponent : public Component {
  String my_mesh_name;
  MeshResourcePtr my_mesh;
  
public:
  MeshComponent(Entity &entity, const String &mesh);
  ~MeshComponent();
  
  void attach() override;
  
  void detach() override;
  
  const MeshResourcePtr& mesh() const;
};

MAP_COMPONENT_TYPE(MeshComponent, MESH)

}