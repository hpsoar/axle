#ifndef AXLE_CG_FWD_H
#define AXLE_CG_FWD_H

#include "../core/settings.h"

namespace ax {
class FlyMotion;
class OrbitMotion;
template<typename MotionType> class PerspectiveCameraGL;
typedef PerspectiveCameraGL<OrbitMotion> OrbitPerspectiveCameraGL;
typedef PerspectiveCameraGL<FlyMotion> FlyPerspectiveCameraGL;

class RenderDeviceFBO;
class FramebufferObject;
class Image;
class CubeImage;
class Texture2D;
class Texture3D;
class CubeTexture;
class ProgramGLSL;
class RenderBuffer;
class ShaderObject;
class ScreenQuad;
class Material;
class GLMaterial;
class GLSLMaterial;
class Scene;
class Object;
class Group;
class GLMesh;
class ArrayBufferGL;

typedef std::tr1::shared_ptr<Image> ImagePtr;
typedef std::tr1::shared_ptr<ShaderObject> ShaderPtr;
typedef std::tr1::shared_ptr<RenderDeviceFBO> FBODevicePtr;
typedef std::tr1::shared_ptr<Texture2D> Texture2DPtr;
typedef std::tr1::shared_ptr<Texture3D> Texture3DPtr;
typedef std::tr1::shared_ptr<ProgramGLSL> ProgramGLSLPtr;
typedef std::tr1::shared_ptr<ScreenQuad> ScreenQuadPtr;
typedef std::tr1::shared_ptr<Material> MaterialPtr;
typedef std::tr1::shared_ptr<GLMaterial> GLMaterialPtr;
typedef std::tr1::shared_ptr<GLSLMaterial> GLSLMaterialPtr;
typedef std::tr1::shared_ptr<Scene> ScenePtr;
typedef std::tr1::shared_ptr<ArrayBufferGL> ArrayBufferGLPtr;

typedef std::tr1::shared_ptr<OrbitPerspectiveCameraGL> 
OrbitPerspectiveCameraGLPtr;

typedef std::tr1::shared_ptr<Object> ObjectPtr;
typedef std::tr1::shared_ptr<Group> GroupPtr;
typedef std::tr1::shared_ptr<GLMesh> GLMeshPtr;
class GLGroup;
typedef std::tr1::shared_ptr<GLGroup> GLGroupPtr;

enum { kUseNormal = 1, kUseTexture = 2 };
enum { 
  kNone = 0,
  kComputeBound = 1, 
  kUseVBO = 2,
  kGenNormal = 4,
  kGenTexcoord = 8,
  kIgnoreGroup = 16,
  kNeedAdjacency = 32
};

class Options {
public:
  Options() : opts_(0) { }
  Options(int opts) : opts_(opts) { }
  void Clear(int opt) { opts_ &= ~opt; }
  void Add(int opt) { this->opts_ |= opt; }
  bool Contain(int opt) const { return (opts_ &opt) != 0; }
  bool IsEmpty() const { return opts_ == kNone; }  
private:
  int opts_;
};

} // ax

#include <GL/glew.h>

#endif // AXLE_CG_FWD_H
