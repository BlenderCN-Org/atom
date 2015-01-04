#pragma once

#include "frame.h"
#include "entity.h"

namespace atom {

typedef std::vector<uptr<Entity>> VectorObjectUPtr;

typedef uptr<Entity> (*ObjectCreateFunc)(World &world, Core &);

typedef Frame* (*CreateFrameFunc)(Core &);
typedef VectorObjectUPtr (*ObjectListFunc)(Core &);


struct EntityDefinition {
  const char *name;
  uptr<Entity> (*create)(World &, Core &core);
};

typedef std::vector<EntityDefinition> (*CreateObjectCreators)(Core &);

struct GameEntry {
  CreateFrameFunc make_first_frame;
  const EntityDefinition *entities;   ///< list of named entity creators, ends with one null record
};

}

extern "C" {
const atom::GameEntry* game_entry();
}
