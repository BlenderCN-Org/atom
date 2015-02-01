#pragma once

#include "ptr.h"
#include "platform.h"

/**
 * This file contains forward declarations of all engine classes/structs plus
 * some useful forward declaration from 3rd party libraries (at the end of file).
 */
namespace atom {

// video
class VideoService;
class Image;
class Sprite;
class Texture;
class VideoBuffer;
class TextureSampler;
class Mesh;
class Model;
class Shader;
class Technique;
class Renderbuffer;
class Framebuffer;
class BitmapFont;
class Uniforms;
class Material;
struct RenderContext;
class MeshTree;
class MeshTreeNode;
struct PixelRGB;
struct PixelRGBA;
struct PixelR32I;
struct PixelBGRA;

// audio
class AudioService;
class AudioBuffer;
class Sound;
class Music;

// system
struct MetaClass;
struct MetaObject;
struct GameEntry;
class Core;
class ResourceService;
class Config;
class SDL;
class LZOProcessor;
class PerformanceCounters;

// resources
class Resource;
class ImageResource;
class TextureResource;
class TechniqueResource;
class MaterialResource;
class ModelResource;
class MeshResource;
class BitmapFontResource;
class SoundResource;
class MusicResource;

struct Bone;

// loaders
struct ResourceLoaders;
class Loader;
class ImageLoader;
class TextureLoader;
class TechniqueLoader;
class MaterialLoader;
class BitmapFontLoader;
class SoundLoader;
class MusicLoader;

// shared resource pointers
typedef sptr<ImageResource> ImageResourcePtr;
typedef sptr<TextureResource> TextureResourcePtr;
typedef sptr<TechniqueResource> TechniqueResourcePtr;
typedef sptr<ModelResource> ModelResourcePtr;
typedef sptr<MeshResource> MeshResourcePtr;
typedef sptr<MaterialResource> MaterialResourcePtr;
typedef sptr<BitmapFontResource> BitmapFontResourcePtr;
typedef sptr<SoundResource> SoundResourcePtr;
typedef sptr<MusicResource> MusicResourcePtr;

// world
class World;
class Entity;
struct UpdateContext;

struct WorldProcessorsRef;

// input
class Gamepad;
class Mouse;
class InputService;
struct Event;
struct KeyEvent;

class Frame;

// components
class Component;
class ModelComponent;
class MaterialComponent;
class MeshComponent;
class RenderComponent;
class ScriptComponent;
class GeometryComponent;
class SkeletonComponent;
class PlaneColliderComponent;
class BoxColliderComponent;
class RigidBodyComponent;
class GenericSlot;

// component utils
class AbstractLink;

class Processor;
class RenderProcessor;
class PhysicsProcessor;
class ScriptProcessor;
class GeometryProcessor;
class DebugProcessor;

// math
class TransformationStack;
struct Transformations;
struct Camera;

}


//
// 3rd party forward declarations
//

// Bullet physics forward declarations
class btDefaultCollisionConfiguration;
class btCollisionDispatcher;
struct btDbvtBroadphase;
class btSequentialImpulseConstraintSolver;
class btDiscreteDynamicsWorld;
