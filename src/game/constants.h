#pragma once

#include <core/types.h>
#include <core/utils/utils.h>

namespace atom {
namespace game {

// collision categories for game objects (16 categories max)
const u16 CATEGORY_MISC          = 1;
const u16 CATEGORY_GROUND        = 1 << 1;
const u16 CATEGORY_PLAYER        = 1 << 2;
const u16 CATEGORY_STICKER       = 1 << 3;
const u16 CATEGORY_ROPE_DETECTOR = 1 << 4;

// collision masks for game objects
// objects may collide only when a.mask & b.category != 0 && b.mask & a.category != 0
const u16 MASK_NOTHING       = 0;
const u16 MASK_GROUND        = CATEGORY_PLAYER | CATEGORY_GROUND;
const u16 MASK_PLAYER        = CATEGORY_GROUND;
const u16 MASK_STICKER       = CATEGORY_ROPE_DETECTOR;
const u16 MASK_ROPE_DETECTOR = CATEGORY_STICKER;

}
}
