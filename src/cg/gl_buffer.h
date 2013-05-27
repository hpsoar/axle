#ifndef AXLE_CG_GL_BUFFER_H
#define AXLE_CG_GL_BUFFER_H

#include "../core.h"
#include "../cg.h"
#include "params.h"

namespace ax {
struct FBParams {
  static const std::string kBufferName;
  static const std::string kTextureTarget;
  static const std::string kColorBufferCount;
  static const std::string kHasDepthBuffer;
  static const std::string kHasStencilBuffer;
  static const std::string kColorFormat;
  static const std::string kDepthFormat;
};

class FrameBuffer {
public:
  struct DEPRECATED(ParamNames) {
    static const std::string kBufferName;
    static const std::string kTextureTarget;
    static const std::string kColorBufferCount;
    static const std::string kHasDepthBuffer;
    static const std::string kHasStencilBuffer;
    static const std::string kColorFormat;
    static const std::string kDepthFormat;
  };

  FrameBuffer() : included_buffers_(0), n_color_buffers_(0), has_stencil_buffer_(false), has_zbuffer_(false) { }

  bool Initialize(const ax::ParamSet &params);

  // @param w > 0, h > 0, reallocate only when size changed
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

  DEPRECATED(ax::Texture2DPtr DepthBuffer() const) {
    return depth_buffer_; 
  }

  const std::vector<ax::Texture2DPtr> color_buffers() const { return this->color_buffers_; }
  ax::Texture2DPtr depth_buffer() const { return this->depth_buffer_; }

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

  // !!! make sure buffer is bind
  ax::Texture2DPtr GrabStencilBuffer();

  ax::Texture2DPtr GrabDepthBuffer();
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
  int depth_format_;
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

private:
  void ComputeMipmapViewports();

  void AdjustViewport(int mipmap_level);

private:
  ax::ScreenQuadPtr screen_quad_;

  std::vector<int> offsets_;
  std::vector<int> widths_;
};

GLuint64 GetGPUPtr(uint32 id, uint32 access, uint32 target=GL_ARRAY_BUFFER);

class ArrayBufferGL {
public:
  static ax::ArrayBufferGLPtr Create(uint32 target, uint32 size, uint32 access,
                                     const void *data) {
    ax::ArrayBufferGLPtr ptr = ax::ArrayBufferGLPtr(new ArrayBufferGL(target));
    if (!ptr->Resize(size, NULL)) return ax::ArrayBufferGLPtr();
    ptr->BindGPUPtr(access);
    return ptr;
  }

  static ax::ArrayBufferGLPtr Create(uint32 size, uint32 access) {
    return ax::ArrayBufferGL::Create(GL_ARRAY_BUFFER, size, access, NULL);
  }

  ArrayBufferGL(int target = GL_ARRAY_BUFFER) : target_(target), capacity_(0), id_(0), gpu_ptr_(0), size_(0) { }

  ~ArrayBufferGL() { this->Release(); }

  void Bind() const { glBindBuffer(this->target_, this->id_); }
  void Unbind() const { glBindBuffer(this->target_, 0); }  
  
  bool Resize(uint32 size, const void *data=NULL, bool force_shrink=false);

  uint32 SetData(int offset, int size, const void *data);

  GLuint64 BindGPUPtr(uint32 access);

  GLuint id() const { return this->id_; }
  GLuint64EXT gpu_ptr() const { return this->gpu_ptr_; }
  uint32 size() const { return this->size_; }
  
private:
  void Release();  
private:
  GLuint id_;
  GLuint64EXT gpu_ptr_;
  uint32 size_;
  uint32 capacity_;
  uint32 target_;
};

}

#endif // AXLE_CG_GL_BUFFER_H
