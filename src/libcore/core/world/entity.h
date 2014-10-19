#pragma once

#include <vector>
#include "../foundation.h"
#include "../math/transformations.h"
#include "../component/component.h"

namespace atom {

typedef sptr<World> WorldPtr;

/**
 * Game entity - entity is each object in world. Entity is composed of components.
 *
 * Each entity should set its bounding box. Bounding box is the max size
 * of the entity with no transformation. AABB is calculated from entity's
 * bounding box and actual transform.
 *
 * Properties:
 *  - no inheritance, don't derive Entity
 *  - clone method (that clones all entity properties)
 *
 * Important methods:
 *  - init
 *  - activate
 *  - deactivate
 *  - terminate
 *  - clone
 *
 * Lifecycle:
 *  - init
 *  - activate
 *  - deactivate
 *  - terminate
 */
class Entity : NonCopyable {
  enum class State {
    NEW,
    INITIALIZED,
    ACTIVATED,
    DEACTIVATED,
    TERMINATED
  };

  typedef std::vector<uptr<Component>> ComponentArray;

  // members
  World         &my_world;
  Core          &my_core;
  State          my_state;
  Mat4f          my_transform;
  BoundingBox    my_bounding_box;
  BoundingBox    my_aabb;
  String         my_id;
  String         my_class;
  ComponentArray my_components;
public:
  Entity(World &world, Core &core);

  ~Entity();

  uptr<Entity> clone(World &world) const;

  /**
   * Entity was added to the world.
   */
  void activate();

  /**
   * Entity is going to be removed from the world.
   */
  void deactivate();

  void add_component(uptr<Component> &&component);

  const String& id() const;

  void set_id(const String &id);

  const Mat4f& transform() const;

  void set_transform(const Mat4f &transform);

  const BoundingBox& bounding_box() const;

  void set_bounding_box(const BoundingBox &box);

  const BoundingBox& aabb() const;

  const String& class_name() const;

  void set_class_name(const String &class_name);

  /// volat len ked je objekt zivy, je zaradeny do nejakeho sveta
  World& world() const;

  Core& core() const;

  Component* find_component(const String &name);

  Component* find_component(ComponentType type);

  Component* find_component(ComponentType type, const String &name);

  template<typename T>
  T* find_component()
  {
    Component *component = find_component(component_type_of<T>());
    return component != nullptr ? static_cast<T *>(component) : nullptr;
  }

  template<typename T>
  T* find_component(const String &name)
  {
    Component *component = find_component(name);
    return component != nullptr ? static_cast<T *>(component) : nullptr;
  }

  std::vector<Component *> find_components(ComponentType type);
  
  template<typename T>
  std::vector<T *> find_components()
  {
    std::vector<T *> components;
    const ComponentType type = component_type_of<T>();
    
    for (const uptr<Component> &component : my_components) {
      if (component->type() == type) {
        components.push_back(static_cast<T *>(component.get()));
      }
    }
    
    return components;
  }
  
  META_ROOT_CLASS;

private:
  void init(const Vec3f &position = Vec3f(0, 0, 0), f32 rotation = 0);

  void update_aabb();
};

}
