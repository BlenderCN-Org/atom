#pragma once

#include <vector>
#include "core/corefwd.h"
#include "core/meta.h"
#include "core/string.h"
#include "core/noncopyable.h"
#include "core/math/mat.h"
#include "core/math/bounding_box.h"
#include "core/math/transformations.h"
#include "core/component/component.h"

namespace atom {

typedef sptr<World> WorldPtr;

/*
 
  Entity
   * activate
   * deactivate
   * clone
  
  Component
   * attach
   * detach
   * duplicate
   * 
   * (v)activate
   * (v)deactivate
   * (v)clone
  
  Slot
   * activate
*/

/**
 * Entita herneho objektu.
 *
 * Notes:
 */
class Entity : NonCopyable {
  enum class State {
    NEW,
    RUNNING
  };

  typedef std::vector<uptr<Component>> ComponentArray;

  // members
  World         &my_world;
  Core          &my_core;
  State          my_state;
  f32            my_width;
  f32            my_height;
  Mat4f          my_transform;
  BoundingBox    my_bounding_box;
  String         my_id;
  String         my_class;
  ComponentArray my_components;
public:

  Entity(World &world, Core &core, f32 width = 1, f32 height = 1);

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

  void set_size(f32 width, f32 height);
  
  const Mat4f& transform() const;
  
  void set_transform(const Mat4f &transform);

  void update_bounding_box();

  const String& class_name() const;

  void set_class_name(const String &class_name);

  /// volat len ked je objekt zivy, je zaradeny do nejakeho sveta
  World &world() const;

  BoundingBox bounding_box() const;

  bool is_live() const;

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
  
public:
  META_DECLARE_CLASS_PTR; // each instance contains pointer to the MetaClass
  META_DECLARE_CLASS;     // static instance of MetaClass for Material

private:
  void init(f32 width, f32 height, const Vec3f &position = Vec3f(0, 0, 0), f32 rotation = 0);

};

}
