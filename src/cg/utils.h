#ifndef AXLE_CG_UTILS_H
#define AXLE_CG_UTILS_H

#include "../core/settings.h"
#include "../core/typedefs.h"
#include "../core/math.h"
#include "../geom.h"

#include <GL/glew.h>
#include "cg_fwd.h"
#include "model_gl.h"
#include "params.h"
#include <stack>
#include "gl_object.h"

#pragma message("WARNING: SLOVE SHADER DEPENDENCY! DO LAZY INITIALIZATION!")

namespace ax {
#ifndef GL_SHADER_GLOBAL_ACCESS_BARRIER_BIT_NV
#define GL_SHADER_GLOBAL_ACCESS_BARRIER_BIT_NV             0x00000010
#endif
class GLContext {
public:
  static bool CheckGLInfo();

  static int MaxImageUnits() { 
    GLContext::Initialize();
    return max_image_units;
  }
private:
  static bool Initialize();

private:
  static bool initialized;
  
  static GLint max_draw_buffers;
  static GLint max_texture_coords;
  static GLint max_textures;
  static GLint max_image_units;
  static GLint max_tex_size;
};

class GPUTimer;
typedef std::tr1::shared_ptr<GPUTimer> GPUTimerPtr;

class GPUTimer : public ax::GLObject {
public:
  static ax::GPUTimerPtr Create(const std::string &name) {
    return ax::GPUTimerPtr(new GPUTimer(name));
  }
  // NOTE: nested call of Begin may lead to error
  void Begin() {
    glBeginQuery(this->target_, this->id());
  }
  double End() {
    glEndQuery(this->target_);
    GLuint64 param = 0;
    glGetQueryObjectui64v(this->id(), GL_QUERY_RESULT, &param);
    return static_cast<double>(param) / 1000000000.0;    
  }
  double End(const std::string &info) {
    double elapsed = this->End();
    printf("---%s end, cost: %f\n", info.c_str(), elapsed);
    return elapsed;
  }
  
  ~GPUTimer() { glDeleteQueries(1, &this->id_); }
protected:
  GPUTimer(const std::string &name) : GLObject(name) {
    this->target_ = GL_TIME_ELAPSED;
    glGenQueries(1, &this->id_);
  }  
  GLuint target_;  
};

class ScopeGPUTimer : private GPUTimer {
public:
  ScopeGPUTimer(const std::string &name) : GPUTimer(name) { 
    this->Begin();
  }
  ~ScopeGPUTimer() {
    this->End(this->name());    
  }
};

class DEPRECATED(GPUTimerManager) {
public:
  void Begin(const std::string &name) {   
    this->timers_.push(ax::GPUTimer::Create(name));
    this->timers_.top()->Begin();
  }
  double End() {
    double elapsed =  this->timers_.top()->End();    
    printf("---%s end, cost: %f\n", this->timers_.top()->name().c_str(), elapsed);
    this->timers_.pop(); 
    return elapsed;
  }  
private:
  std::stack<ax::GPUTimerPtr> timers_;    
};

enum StatisticsOption {
  kStatNone = 0,
  kStatTime = 1
};

class ScopeStatistics {
public:
  ScopeStatistics(const std::string &name, ax::Options opts) : name_(name), opts_(opts) {
    if (opts.Contain(kStatTime)) {
      this->timer_ = ax::GPUTimer::Create(name);
      if (this->timer_ != NULL) this->timer_->Begin();
    }
  }
  ~ScopeStatistics() {
    if (this->timer_ != NULL) {
      this->timer_->End(this->name_);
    }
  }
  GPUTimerPtr timer_;
  ax::Options opts_;
  const std::string name_;
};

class RenderAppI;
typedef std::tr1::shared_ptr<RenderAppI> RenderAppPtr;
class RenderAppI {
public:  
  static ax::RenderAppPtr Create(
      RenderAppI *app, const ax::ParamSet2 &params = ax::ParamSet2()) {
    ax::RenderAppPtr ptr = ax::RenderAppPtr(app);
    if (ptr != NULL && ptr->Initialize(params)) return ptr;
    return NULL;
  }
  RenderAppI(const std::string &name) : name_(name), width_(0), height_(0) { }
  virtual ~RenderAppI() { }  
  virtual void Resize(int w, int h) { this->width_ = w, this->height_ = h; }
  virtual void Render(const ax::ScenePtr scene, const ax::ParamSet2 &params) = 0;

  virtual std::string description() const { return this->name(); }

  const std::string &name() const { return name_; }
  int width() const { return this->width_; }
  int height() const { return this->height_; }  
private:
  virtual bool Initialize(const ax::ParamSet2 &params) = 0;
private:
  std::string name_;
  int width_, height_;

  DISABLE_COPY_AND_ASSIGN(RenderAppI);
};

GDEF_PARAM_NAME(kLightPositionParam);
GDEF_PARAM_NAME(kMVPMatrixParam);
GDEF_PARAM_NAME(kMVMatrixParam);
GDEF_PARAM_NAME(kSceneOptionParam);
GDEF_PARAM_NAME(kStatisticsOptionParam);

class TextureUtil {
public:
  TextureUtil() { }

  bool Initialize();
  // TODO: move out
  void CreateMaxDepthDerivativeTexture(ax::Texture2DPtr position_tex,
                                       ax::Texture2DPtr texture);
  void CreateMinMaxNormalTexture(ax::Texture2DPtr normal_tex,
                                 ax::Texture2DPtr texture);  
    
  static void ReduceMinMax(ax::ImagePtr img, float *ret);  

  //NOTE: support Texture2D only, require power of two size
  static int CreateCustomMipmap(ax::ProgramGLSLPtr shader, ax::Texture2DPtr texture, int min_res = 2);

  static void ReduceMinMax(ax::Texture2DPtr tex, float *ret);

private:
  static bool Initiaialze();

  static ax::FBODevicePtr device_;
  static ax::ScreenQuadPtr quad_;  

  ax::ProgramGLSLPtr max_depth_derivative_prog_;
  ax::ProgramGLSLPtr min_max_normal_prog_;  

  DISABLE_COPY_AND_ASSIGN(TextureUtil);
};

enum {
  kCopyTextureRect,
  kCopyTexture2D,  
};

class TextureCopier {
public:  
  static void Copy(ax::Texture2DPtr texture);
  static void Copy(ax::Texture2DPtr, int width, int height);
private:
  static void DrawFullScreenQuad(ax::ProgramGLSLPtr prog);
  static void CopyTexture2D(ax::Texture2DPtr texture);
  static void CopyTextureRect(ax::Texture2DPtr texture);
};


void DrawTextGL(int x, int y, const char *text);
void DisplayStatistics(const char *name, float data, const char *unit,
                       int x = 2, int y = 9);
GLuint MakeFullScreenQuad();

void VisualizeTexture(const Texture2DPtr tex, bool full_screen = false);
void SaveFrameBuffer(const char *filename);
/*
 * returns ture if error ocurred, otherwise return false
 */
bool CheckErrorsGL(const char *location);

inline bool CheckGLInfo() { return GLContext::CheckGLInfo(); }

template<typename T>
void EmitVertex(const T &p) { glVertex3f(p.x, p.y, p.z); }

template<typename C>
void VisualizeViewSpace(const C &camera, float alpha = 0.5f) {
  float fovy_2 = ax::deg2rad(camera.fovy()) * 0.5f;
  float tan_fovy_2 = tan(fovy_2);
  float near_half_len = tan_fovy_2 * camera.z_near();
  float far_half_len = tan_fovy_2 * camera.z_far();
  float x[] = { -1, 1, 1, -1 };
  float y[] = { -1, -1, 1, 1 };
  ax::Vector3 far_plane[4];
  ax::Vector3 near_plane[4];
  for (int i = 0; i < 4; ++i) {
    far_plane[i] = ax::Vector3(x[i] * far_half_len,
                                 y[i] * far_half_len,
                                 camera.z_far());
    near_plane[i] = ax::Vector3(x[i] * near_half_len,
                                  y[i] * near_half_len,
                                  camera.z_near());
  }

  glPushAttrib(GL_ENABLE_BIT);
  glEnable(GL_BLEND);
  glDisable(GL_CULL_FACE);
  glBlendFunc(GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA);

  glColor4f(1, 1, 1, alpha);
  glPushMatrix();

  glTranslatef(camera.position().x,
               camera.position().y,
               camera.position().z);
  ax::Vector3 look = camera.look();
  float z_angle = ax::rad2deg(acos((look.z) / glm::length(look)));
  float x_angle = ax::rad2deg(atan2((look.y), look.x));  
  glRotated(x_angle, 0, 0, 1);
  glRotated(z_angle, 0, 1, 0);
  
  glBegin(GL_QUADS);
  for (int i = 0; i < 4; ++i) EmitVertex(near_plane[i]);
  for (int i = 0; i < 4; ++i) EmitVertex(far_plane[3 - i]);

  glColor4f(1, 0, 0, alpha);
  EmitVertex(near_plane[0]);
  EmitVertex(near_plane[3]);
  EmitVertex(far_plane[3]);
  EmitVertex(far_plane[0]);

  glColor4f(0, 1, 0, alpha);
  EmitVertex(near_plane[3]);
  EmitVertex(near_plane[2]);
  EmitVertex(far_plane[2]);
  EmitVertex(far_plane[3]);

  glColor4f(0, 0, 1, alpha);
  EmitVertex(near_plane[2]);
  EmitVertex(near_plane[1]);
  EmitVertex(far_plane[1]);
  EmitVertex(far_plane[2]);

  glColor4f(0.8f, 0.5f, 0.2f, alpha);
  EmitVertex(near_plane[1]);
  EmitVertex(near_plane[0]);
  EmitVertex(far_plane[0]);
  EmitVertex(far_plane[1]);
  
  glEnd();
  //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  glPopMatrix();
  glBegin(GL_LINES);
  EmitVertex(camera.position());
  EmitVertex(camera.look() * 100.f);
  glEnd();

  glPopAttrib();
}

inline void SetFrameBufferMask(GLboolean enable_color = GL_TRUE, GLboolean enable_depth = GL_TRUE) {
  glColorMask(enable_color, enable_color, enable_color, enable_color);
  glDepthMask(enable_depth);
}
} // ax

#endif // AXLE_CG_UTILS_H
