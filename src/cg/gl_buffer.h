#ifndef AXLE_GRAPHICS_GL_BUFFER_H
#define AXLE_GRAPHICS_GL_BUFFER_H

#include "axle/core.h"
#include "axle/cg.h"
#include "axle/cg/params.h"

namespace ax {
class FrameBuffer {
public:
  struct ParamNames {
    static const std::string kBufferName;
    static const std::string kTextureTarget;
    static const std::string kColorBufferCount;
    static const std::string kHasDepthBuffer;
    static const std::string kHasStencilBuffer;
    static const std::string kColorFormat;
  }; 

  FrameBuffer() : included_buffers_(0) { }

  bool Initialize(const ax::ParamSet &params);

  bool Resize(int w, int h);

  // !!! unsafe
  void ClearBuffers() { glClear(included_buffers_); }

  void BindAsRenderTarget() { 
    this->device_->Activate();   
    this->device_->AdjustViewport(this->width(), this->height());
  }
 
  // !!! no checking, unsafe
  void Unbind() { this->device_->Deactivate(); }

  ax::Texture2DPtr ColorBuffer(int i) const { return this->color_buffers_[i]; }
  ax::Texture2DPtr DepthBuffer() const { return depth_buffer_; }

  int width() const {
    if (this->depth_buffer_ != NULL) return this->depth_buffer_->width();
    if (!this->color_buffers_.empty()) return this->color_buffers_[0]->width();
    return 0;
  }

  int height() const {
    if (this->depth_buffer_ != NULL) return this->depth_buffer_->height();
    if (!this->color_buffers_.empty()) return this->color_buffers_[0]->height();
    return 0;
  }

protected:
  ax::FBODevicePtr device_;

  std::vector<ax::Texture2DPtr> color_buffers_;
  ax::Texture2DPtr depth_buffer_;
private:
  int included_buffers_;

  std::string name_;

  int n_color_buffers_;
  bool has_zbuffer_;
  bool has_stencil_buffer_;
  int texture_target_;
  int color_format_;
};

// TODO: change to support non-square buffer
class MultiResolutionBuffer : public FrameBuffer {
public:
  bool Resize(int w, int h);

  bool Initialize(const ax::ParamSet &params);

  int MipmapLevel(int resolution) const {
    return ax::Log2(this->height()) - ax::Log2(resolution);
  }

  int MipmapXOffset(int level) const { return this->offsets_[level]; }
  int MipmapWidth(int level) const { return this->widths_[level]; }

  void RenderFullScreen(ax::ProgramGLSLPtr shader);
  void RenderFullScreen(ax::ProgramGLSLPtr shader, int mipmap_level);
  void ClearColorBuffer(ax::ProgramGLSLPtr shader);

  // !!! make sure buffer is bind
  ax::Texture2DPtr GrabStencilBuffer();

private:
  void ComputeMipmapViewports();

  void AdjustViewport(int mipmap_level);

private:
  ax::ScreenQuadPtr screen_quad_;

  std::vector<int> offsets_;
  std::vector<int> widths_;
};

class TextureUtil {
public:
  bool Initialize();
  int CreateCustomMipmap(ax::ProgramGLSLPtr shader, ax::Texture2DPtr texture,
                         int min_res = 2);
  void CreateMaxDepthDerivativeTexture(ax::Texture2DPtr position_tex,
                                       ax::Texture2DPtr texture);
  void CreateMinMaxNormalTexture(ax::Texture2DPtr normal_tex,
                                 ax::Texture2DPtr texture);  
  void Reduce(ax::Texture2DPtr texture, float *ret, int n);
private:
  ax::FBODevicePtr device_;
  ax::ScreenQuadPtr quad_;

  ax::ProgramGLSLPtr max_depth_derivative_prog_;
  ax::ProgramGLSLPtr min_max_normal_prog_;

  ax::ProgramGLSLPtr reduction_prog_;
};

}

#endif // AXLE_GRAPHICS_GL_BUFFER_H