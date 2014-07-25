#pragma once

#include "string.h"
#include "types.h"

namespace atom {

const char CONFIG_FILENAME[] = "data/config";

const int DEFAULT_SCREEN_WIDTH = 960;
const int DEFAULT_SCREEN_HEIGHT = 544;
const int DEFAULT_SCREEN_BITS_PER_PIXEL = 32;
const float DEFAULT_SCREEN_ASPECT_RATIO = DEFAULT_SCREEN_WIDTH / (float) DEFAULT_SCREEN_HEIGHT;
const int FPS = 30;
const int MAX_SKIP_FRAME = 3;
const float DEFAULT_WORLD_WIDTH = 6;
const float DEFAULT_WORLD_HEIGHT = (float) DEFAULT_WORLD_WIDTH / DEFAULT_SCREEN_ASPECT_RATIO;
const String DEFAULT_SHADER_DIR("data/shader");

const int AUDIO_FREQUENCY = 44100;
const u32 AUDIO_SAMPLES = 2048;  ///< desired buffer size (in samples)
const u32 AUDIO_CHANNELS = 2;    ///< stereo
const u32 AUDIO_SAMPLE_SIZE = 2;  ///< sample size is 2bytes (i16)

const float ACCELERATION = 9.81; // acceleration constant

const char DATA_DIR[] = "data";
const char IMAGE_RESOURCE_DIR[] = "data/image"; ///< adresar s obrazkami
const char SOUND_RESOURCE_DIR[] = "data/sound"; ///< adresar so zvukmi
const char MUSIC_RESOURCE_DIR[] = "data/music"; ///< adresar s pesnickami
const char MESH_RESOURCE_DIR[] = "data/mesh"; ///< adresar s pesnickami
const char SHADER_RESOURCE_DIR[] = "data/shader";
const char LEVEL_RESOURCE_DIR[] = "data/level";
const char MATERIAL_DIR[] = "data/material";

const char LEVEL_FILE_EXT[] = "lev";
const char MATERIAL_EXT[] = "mat";
const char MESH_EXT[] = "m3d";

const int PATH_SIZE = 256;

const int PNG_SIGNATURE_SIZE = 8; ///< velkost PNG signatury, nemenit

const int PROPERTIES_MAX_KEY_SIZE = 100;
const int PROPERTIES_MAX_VALUE_SIZE = 100;

// keys used in the config file
const char SCREEN_WIDTH_KEY[] = "screen_width";
const char SCREEN_HEIGHT_KEY[] = "screen_height";
const char WORLD_WIDTH_KEY[] = "world_width";
const char WORLD_HEIGHT_KEY[] = "world_height";

const float PLAYER_WIDTH = 0.8;
const float PLAYER_HEIGHT = 1.3;

const float BALL_WEIGHT = 5;

const char RESOURCE_DELIMITER = ':';
const char RESOURCE_FILE_TAG[] = "file";
const char RESOURCE_IMAGE_TAG[] = "image";
const char RESOURCE_TEXTURE_TAG[] = "texture";
const char RESOURCE_SHADER_TAG[] = "shader";
const char RESOURCE_MESH_TAG[] = "mesh";
const char RESOURCE_MODEL_TAG[] = "model";
const char RESOURCE_MATERIAL_TAG[] = "material";
const char RESOURCE_BITMAP_FONT_TAG[] = "bitmap_font";
const char RESOURCE_SOUND_TAG[] = "sound";
const char RESOURCE_MUSIC_TAG[] = "music";

const int REQ_VERTEX_ATTRIBUTES = 4;

}
