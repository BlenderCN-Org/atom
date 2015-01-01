#pragma once

#include "frame.h"
#include "entity.h"

namespace atom {

typedef std::vector<uptr<Entity>> VectorObjectUPtr;

typedef uptr<Entity> (*ObjectCreateFunc)(World &world, Core &);

struct EntityCreator {
  String           name;
  ObjectCreateFunc create;

  EntityCreator(const String &object_name, ObjectCreateFunc create_func)
    : name(object_name)
    , create(create_func)
  {

  }
};

typedef Frame* (*CreateFrameFunc)(Core &);
typedef VectorObjectUPtr (*ObjectListFunc)(Core &);
typedef std::vector<EntityCreator> (*CreateObjectCreators)(Core &);

struct GameEntry {
  CreateFrameFunc make_first_frame;
  CreateObjectCreators make_entity_creators;
};

}

extern "C" {
const atom::GameEntry* game_entry();
}
