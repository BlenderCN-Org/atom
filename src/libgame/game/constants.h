#pragma once

#include <core/foundation.h>

namespace atom {

namespace CollisionMask {

const u32 WORLD  = 0x00000001;
const u32 PLAYER = 0x00000002;
const u32 ENEMY  = 0x00000004;
const u32 ALL    = 0xFFFFFFFF;

}

}