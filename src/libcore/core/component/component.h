#pragma once

#include <cassert>
#include <vector>
#include "../foundation.h"

namespace atom {

enum class ComponentType {
  UNKNOWN,
  MATERIAL,
  RENDER,
  MESH,
  RAW_MESH,
  SCRIPT,
  RIGID_BODY,
  SKELETON
};

typedef std::vector<GenericSlot *> SlotArray;

class Component : NonCopyable {
  ComponentType my_type;
  Entity       *my_entity;
  String        my_name;
  SlotArray     my_slots;

  virtual void activate() = 0;

  virtual void deactivate() = 0;

  virtual uptr<Component> clone() const = 0;

public:
  Component(ComponentType type);
  virtual ~Component();

  uptr<Component> duplicate() const;

  void attach(Entity &entity);
  void detach();

  // volat len po welcome a pred goodbye
  Entity& entity() const;

  World& world() const;

  Core& core() const;

  WorldProcessorsRef processors() const;

  ComponentType type() const;

  const String& name() const;

  void set_name(const String &name);

  void register_slot(GenericSlot *slot);
};

template<typename T>
ComponentType component_type_of();

#define MAP_COMPONENT_TYPE(type, mapped)  \
  template<>                    \
  inline ComponentType component_type_of<type>()   \
  { return ComponentType::mapped; }



class GenericSlot : NonCopyable {
  Component    *my_component;
  ComponentType my_type;
  String        my_name;  ///< component name

public:
  explicit GenericSlot(Component *parent, ComponentType type,
    const String &name = String())
    : my_component(nullptr)
    , my_type(type)
    , my_name(name)
  {
    assert(parent != nullptr);
    parent->register_slot(this);
  }

  void activate(Entity &entity);

  Component* get() const;

  operator bool() const
  {
    return my_component != nullptr;
  }

};

template<typename T>
class Slot : public GenericSlot {
public:
  explicit Slot(Component *parent, const String &name = String())
    : GenericSlot(parent, component_type_of<T>(), name)
  {
    // empty
  }

  T* operator->() const
  {
    T* component = static_cast<T *>(get());
    assert(component != nullptr);
    return component;
  }
};






}
