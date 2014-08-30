#pragma once

#include "ptr.h"
#include "platform.h"

namespace atom {

// video
class VideoService;
class DrawService;
class Image;
class Sprite;
class Texture;
class VideoBuffer;
class TextureSampler;
class Mesh;
class RawMesh;
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

typedef sptr<Framebuffer> FramebufferPtr;
typedef sptr<Renderbuffer> RenderbufferPtr;

// audio
class AudioService;
class AudioBuffer;
class Sound;
class Music;

// system
struct MetaClass;
struct MetaObject;
struct GameAPI;
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
class RawMeshResource;
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
typedef sptr<RawMeshResource> RawMeshResourcePtr;
typedef sptr<MeshResource> MeshResourcePtr;
typedef sptr<MaterialResource> MaterialResourcePtr;
typedef sptr<BitmapFontResource> BitmapFontResourcePtr;
typedef sptr<SoundResource> SoundResourcePtr;
typedef sptr<MusicResource> MusicResourcePtr;

// world
class World;
class Entity;
struct UpdateContext;
typedef sptr<World> WorldPtr;

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
class MaterialComponent;
class MeshComponent;
class RenderComponent;
class ScriptComponent;
class SkeletonComponent;
class PlaneColliderComponent;
class BoxColliderComponent;
class RigidBodyComponent;
class GenericSlot;

// component utils
class AbstractLink;

class VideoProcessor;
class PhysicsProcessor;
class ScriptProcessor;

// math
class TransformationStack;
struct Transformations;
struct Camera;

}
