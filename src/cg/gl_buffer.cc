#include "gl_buffer.h"

namespace ax {
const std::string ax::FBParams::kBufferName = "frame buffer name";
const std::string ax::FBParams::kTextureTarget = "frame buffer texture target";
const std::string ax::FBParams::kColorBufferCount = "frame buffer color buffer count";
const std::string ax::FBParams::kHasDepthBuffer = "frame buffer has depth buffer";
const std::string ax::FBParams::kHasStencilBuffer = "frame buffer has stencil buffer";
const std::string ax::FBParams::kColorFormat = "frame buffer color format";
const std::string ax::FBParams::kDepthFormat = "frame buffer depth format";

const std::string FrameBuffer::ParamNames::kBufferName = ax::FBParams::kBufferName;
const std::string FrameBuffer::ParamNames::kTextureTarget = ax::FBParams::kTextureTarget;
const std::string FrameBuffer::ParamNames::kColorBufferCount = ax::FBParams::kColorBufferCount;
const std::string FrameBuffer::ParamNames::kHasDepthBuffer = ax::FBParams::kHasDepthBuffer;
const std::string FrameBuffer::ParamNames::kHasStencilBuffer = ax::FBParams::kHasStencilBuffer;
const std::string FrameBuffer::ParamNames::kColorFormat = ax::FBParams::kColorFormat;
const std::string FrameBuffer::ParamNames::kDepthFormat = ax::FBParams::kDepthFormat;

bool FrameBuffer::Initialize(const ax::ParamSet &params) {
  this->name_ = params.GetStr(ax::FBParams::kBufferName, "frame buffer");
  this->texture_target_ = params.GetInt(ax::FBParams::kTextureTarget, GL_TEXTURE_2D);
  this->color_format_ = params.GetInt(ax::FBParams::kColorFormat, GL_RGBA);
  this->n_color_buffers_ = params.GetInt(ax::FBParams::kColorBufferCount, 1);  
  this->has_stencil_buffer_ = params.GetBool(ax::FBParams::kHasStencilBuffer, false);
  this->has_zbuffer_ = params.GetBool(ax::FBParams::kHasDepthBuffer, false);

  this->depth_format_ = params.GetInt(ax::FBParams::kDepthFormat, 0);
  if (this->depth_format_ == 0) {
    if (this->has_stencil_buffer_) {
      // it seems that stencil buffer ought to has depth buffer with it
      this->depth_format_ = GL_DEPTH24_STENCIL8;
    } else if (this->has_zbuffer_) {
      this->depth_format_ = GL_DEPTH_COMPONENT;
    }
  }

  this->included_buffers_ = 0;
  if (this->n_color_buffers_) this->included_buffers_ |= GL_COLOR_BUFFER_BIT;
  if (this->has_zbuffer_ || this->has_stencil_buffer_) 
    this->included_buffers_ |= GL_DEPTH_BUFFER_BIT;
  if (this->has_stencil_buffer_) 
    this->included_buffers_ |= GL_STENCIL_BUFFER_BIT;

  return true;
}

bool FrameBuffer::Resize(int w, int h) { 
  V_RET(w > 0 && h > 0);
  if (this->width() == w && this->height() == h) return true;

  if (this->included_buffers_ == 0) {
    ax::Logger::Log("FrameBuffer::Resize: no buffer is included");
    return false;
  }

  if (this->device_ == NULL) {
    V_RET(this->device_ = ax::RenderDeviceFBO::Create());
  }

  if (this->n_color_buffers_ > 0) {
    this->color_buffers_.resize(this->n_color_buffers_);
    for (int i = 0; i < this->n_color_buffers_; ++i) {
      V_RET(this->color_buffers_[i] = ax::Texture2D::Create(
            this->texture_target_, this->name_));
      V_RET(this->color_buffers_[i]->Initialize(w, h, this->color_format_));
      this->color_buffers_[i]->SetDefaultParameters();
    }
  }

  if (this->has_stencil_buffer_ || this->has_zbuffer_) {
    V_RET(this->depth_buffer_ = ax::Texture2D::Create(
          this->texture_target_, this->name_ + " zbuffer"));
    V_RET(this->depth_buffer_->InitializeDepth(w, h, this->depth_format_));
    this->depth_buffer_->SetDefaultParameters();
  }

  this->device_->Activate();
  if (this->n_color_buffers_ > 0)
    this->device_->SetRenderTargets(this->color_buffers_);
  else
    this->device_->DisableColorBuffer();

  if (this->has_zbuffer_ || this->has_stencil_buffer_)
    this->device_->SetDepthBuffer(this->depth_buffer_);  
  else
    this->device_->DisableDepthBuffer();

  if (this->has_stencil_buffer_) 
    this->device_->SetStencilBuffer(this->depth_buffer_);
  this->device_->Deactivate();  

  this->device_->Check();

  return !ax::CheckErrorsGL("FrameBuffer::Resize");
}

ax::Texture2DPtr FrameBuffer::GrabStencilBuffer() {
  this->BindAsRenderTarget();
  int w = this->width();
  int h = this->height();

  assert(w > 0 && h > 0);
  if (w <= 0 || h <= 0) return ax::Texture2DPtr();

  char *data = new char[w * h];
  glReadPixels(0, 0, w, h, GL_STENCIL_INDEX, GL_UNSIGNED_BYTE, data);
  
  ax::ImagePtr image = ax::Image::Create(
      w, h, GL_LUMINANCE, GL_UNSIGNED_BYTE, data);

  ax::Texture2DPtr texture = ax::Texture2D::Create2D("stencil texture");

  if (texture != NULL && texture->Initialize(*image, GL_RGBA)){
    texture->SetDefaultParameters();
    this->Unbind();
  }

  return texture;
}

/*
 * class MultiResolutionBuffer
 */
bool MultiResolutionBuffer::Initialize(const ax::ParamSet &params) {
  V_RET(FrameBuffer::Initialize(params));

  V_RET(this->screen_quad_ = ax::ScreenQuad::Create());

  return true;
}

bool MultiResolutionBuffer::Resize(int w, int h) {
  V_RET(w == h);

  V_RET(FrameBuffer::Resize(w * 2, h));

  this->ComputeMipmapViewports();

  return true;
}

void MultiResolutionBuffer::ClearColorBuffer(ax::ProgramGLSLPtr shader) {
  /*shader->Begin();
  shader->Set4DMatVar("mvp_mat", this->screen_quad_->mvp());
  this->BindAsRenderTarget();
  this->device_->AdjustViewport(this->);
  this->screen_quad_->Draw();  
  shader->End();*/
}

void MultiResolutionBuffer::ComputeMipmapViewports() {
  int width = this->height();
  int offset = 0;

  int n_levels = ax::Log2(width);
  this->offsets_.resize(n_levels);
  this->widths_.resize(n_levels);
  for (int i = 0; i < n_levels; ++i) {
    this->offsets_[i] = offset;
    this->widths_[i] = width;
    offset += width;
    width /= 2;
  }
}

void MultiResolutionBuffer::AdjustViewport(int mipmap_level) {
  glViewport(this->MipmapXOffset(mipmap_level), 0,
             this->MipmapWidth(mipmap_level),
             this->MipmapWidth(mipmap_level));
}

void MultiResolutionBuffer::RenderFullScreen(ax::ProgramGLSLPtr shader) {
  shader->Set4DMatVar("g_mvp_mat", this->screen_quad_->mvp());

  for (int i = 0; i < 6; ++i) {
    shader->SetVar("g_mipmap_level", i);
    this->AdjustViewport(i);
    this->screen_quad_->Draw();
  }
}

void MultiResolutionBuffer::RenderFullScreen(
    ax::ProgramGLSLPtr shader, int mipmap_level) {
  shader->Set4DMatVar("g_mvp_mat", this->screen_quad_->mvp());
  shader->SetVar("g_mipmap_level", mipmap_level);
  this->AdjustViewport(mipmap_level);
  this->screen_quad_->Draw();
}

//// ArrayBufferGL

GLuint64 GetGPUPtr(uint32 id, uint32 access, uint32 target) {
  glBindBuffer(target, id);

  GLuint64 gpu_ptr = 0;
  glMakeBufferResidentNV(target, access);
  glGetBufferParameterui64vNV(target, GL_BUFFER_GPU_ADDRESS_NV, &gpu_ptr);

  glBindBuffer(target, 0); 

  return gpu_ptr;
}

bool ArrayBufferGL::Resize(uint32 size, const void *data, bool force_shrink) {
  V_RET(size > 0);
  if (this->capacity_ < size || force_shrink) {
    this->Release();    
    glGenBuffers(1, &this->id_);
    this->Bind();    
    glBufferData(this->target_, size, data, GL_STREAM_DRAW); // TODO:    
    this->Unbind();
    if (ax::CheckErrorsGL("ArrayBufferGL::Resize")) {
      ax::Logger::Log("ArrayBufferGL::Resize: old buffer is also unavailable");
      return false;
    }    
    this->capacity_ = size;
  }
  this->size_ = size;
  return true;
}

GLuint64 ArrayBufferGL::BindGPUPtr(uint32 access) {
  this->Bind();  
  glMakeBufferResidentNV(this->target_, access);  
  glGetBufferParameterui64vNV(this->target_, GL_BUFFER_GPU_ADDRESS_NV,
                              &this->gpu_ptr_);  
  this->Unbind();
  ax::CheckErrorsGL("ArrayBufferGL::BindGPUPtr");
  return this->gpu_ptr_;
}

void ArrayBufferGL::Release() {
  if (this->id_ > 0) {
    glDeleteBuffers(1, &this->id_);
    this->id_ = 0;
    this->gpu_ptr_ = 0;
    this->size_ = 0;
    this->capacity_ = 0;
  }
}

uint32 ArrayBufferGL::SetData(int offset, int size, const void *data) {
  this->Bind();

  glBufferSubData(this->target_, offset, size, data);  

  this->Unbind();

  return size;
}
}
