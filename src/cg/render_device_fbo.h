#ifndef AXLE_GRAPHICS_RENDER_DEVICE_H
#define AXLE_GRAPHICS_RENDER_DEVICE_H

#include "axle/core/settings.h"

#include <vector>

#include "axle/cg/framebuffer_object.h"
#include "axle/cg/cg_fwd.h"

namespace ax {
class Texture2D;
class RenderBuffer;
class RenderDeviceFBO {
 public:
  static FBODevicePtr Create() {
    return FBODevicePtr(new RenderDeviceFBO());
  }

  void Activate() { fbo_.Bind(); }
  void Deactivate() { FramebufferObject::Disable(); }
  void Check() { fbo_.IsValid(); }

  void SetRenderTarget(const Texture2DPtr texture, int level = 0);
  void SetRenderTargets(const std::vector<Texture2DPtr> &textures, int level = 0);
  void SetRenderTarget(const RenderBuffer *render_buffer);
  void SetRenderTargets(int target_counts, 
                        const RenderBuffer *const render_buffers[]);

  void SetDepthBuffer(const Texture2DPtr texture);
  void SetDepthBuffer(const RenderBuffer *render_buffer);

  void SetStencilBuffer(const Texture2DPtr texture);

  void DisableDepthBuffer();
  void DisableColorBuffer();
  void DisableStencilBuffer();

  template<typename T>
  void AdjustViewport(const T &target) {
    this->AdjustViewport(target->width(), target->height());
  }
  void AdjustViewport(int w, int h) { glViewport(0, 0, w, h); }
  /*
   * there may be problem in the case of multiple render targets,
   * if simply disable frame buffer object, maybe here should record them
   * and detach them when necessary, for example, when target_count exceeds
   * the target_count in last call to SetRenderTargetGL
   */
  void SaveMVP();
  void RestoreMVP();
private:
  RenderDeviceFBO() { }

private:
  FramebufferObject fbo_;
  GLint s_viewport[4];
  static GLenum s_mrt[8];
};

} // ax

#endif // AXLE_GRAPHICS_RENDER_DEVICE_H
