#include "gl_buffer.h"

namespace ax {
const std::string FrameBuffer::ParamNames::kBufferName = "frame buffer name";
const std::string FrameBuffer::ParamNames::kTextureTarget = "frame buffer texture target";
const std::string FrameBuffer::ParamNames::kColorBufferCount = "frame buffer color buffer count";
const std::string FrameBuffer::ParamNames::kHasDepthBuffer = "frame buffer has depth buffer";
const std::string FrameBuffer::ParamNames::kHasStencilBuffer = "frame buffer has stencil buffer";
const std::string FrameBuffer::ParamNames::kColorFormat = "frame buffer color format";

bool FrameBuffer::Initialize(const ax::ParamSet &params) {
  this->name_ = params.GetStr(ParamNames::kBufferName, "frame buffer");
  this->texture_target_ = params.GetInt(ParamNames::kTextureTarget, GL_TEXTURE_2D);
  this->color_format_ = params.GetInt(ParamNames::kColorFormat, GL_RGBA);
  this->n_color_buffers_ = params.GetInt(ParamNames::kColorBufferCount, 1);
  this->has_stencil_buffer_ = params.GetBool(ParamNames::kHasStencilBuffer, false);
  this->has_zbuffer_ = params.GetBool(ParamNames::kHasDepthBuffer, false);

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

  if (this->device_ == NULL) {
    V_RET(this->device_ = ax::RenderDeviceFBO::Create());
  }

  this->color_buffers_.resize(this->n_color_buffers_);
  for (int i = 0; i < this->n_color_buffers_; ++i) {
    V_RET(this->color_buffers_[i] = ax::Texture2D::Create(
          this->texture_target_, this->name_));
    V_RET(this->color_buffers_[i]->Initialize(w, h, this->color_format_));
    this->color_buffers_[i]->SetDefaultParameters();
  }

  int depth_format = 0;
  if (this->has_stencil_buffer_) {
    // it seems that stencil buffer ought to has depth buffer with it
    depth_format = GL_DEPTH24_STENCIL8;
  } else if (this->has_zbuffer_) {
    depth_format = GL_DEPTH_COMPONENT;
  }  

  if (this->has_stencil_buffer_ || this->has_zbuffer_) {
    V_RET(this->depth_buffer_ = ax::Texture2D::Create(
          this->texture_target_, this->name_ + " zbuffer"));
    V_RET(this->depth_buffer_->InitializeDepth(w, h, depth_format));
  }

  this->device_->Activate();
  this->device_->SetRenderTargets(this->color_buffers_);
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

ax::Texture2DPtr MultiResolutionBuffer::GrabStencilBuffer() {
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

void MultiResolutionBuffer::RenderFullScreen(ax::ProgramGLSLPtr shader) {
  shader->Set4DMatVar("mvp_mat", this->screen_quad_->mvp());

  for (int i = 0; i < 6; ++i) {
    shader->SetVar("g_mipmap_level", i);
    this->AdjustViewport(i);
    this->screen_quad_->Draw();
  }
}

void MultiResolutionBuffer::RenderFullScreen(
    ax::ProgramGLSLPtr shader, int mipmap_level) {
  shader->Set4DMatVar("mvp_mat", this->screen_quad_->mvp());
  shader->SetVar("g_mipmap_level", mipmap_level);
  this->AdjustViewport(mipmap_level);
  this->screen_quad_->Draw();
}

bool TextureUtil::Initialize() {
  V_RET(this->device_ = ax::RenderDeviceFBO::Create());
  V_RET(this->quad_ = ax::ScreenQuad::Create());

  V_RET(this->max_depth_derivative_prog_ = ax::ProgramGLSL::Create(
      "shaders/quad.vp", "shaders/max_depth_derivative.fp", 
      "max depth derivative"));
  V_RET(this->max_depth_derivative_prog_->Link());

  V_RET(this->min_max_normal_prog_ = ax::ProgramGLSL::Create(
      "shaders/quad.vp", "shaders/min_max_normal.fp", "min max normal"));
  V_RET(this->min_max_normal_prog_->Link());

  V_RET(this->reduction_prog_ = ax::ProgramGLSL::Create(
      "shaders/quad.vp", "shaders/min_max_depth_mipmap.fp", "depth mipmap"));
  V_RET(this->reduction_prog_->Link());

  return true;
}

int TextureUtil::CreateCustomMipmap(ax::ProgramGLSLPtr shader, 
                                    ax::Texture2DPtr texture,
                                    int min_res) {
  glPushAttrib(GL_ENABLE_BIT);
  glDisable(GL_DEPTH_TEST);

  this->device_->Activate();
  this->device_->SaveMVP();

  shader->Begin();
  shader->Set4DMatVar("mvp_mat", this->quad_->mvp());
  shader->SetTextureVar("g_input_tex", texture);
  
  float step = 1.0f;
  int width = texture->width() / 2;
  int level = 1;
  
  texture->Bind();  

  while (width >= min_res) {
    this->device_->SetRenderTarget(texture, level);
    this->device_->AdjustViewport(width, width);
    glClear(GL_COLOR_BUFFER_BIT);

    texture->SetParameter(GL_TEXTURE_BASE_LEVEL, level - 1);
    texture->SetParameter(GL_TEXTURE_MAX_LEVEL, level - 1);
    texture->SetParameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    texture->SetParameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);    

    shader->SetVar("g_offset", step / texture->width());

    this->quad_->Draw();

    ++level;
    step *= 2;
    width /= 2;
  }
  texture->SetParameter(GL_TEXTURE_BASE_LEVEL, 0);  
  texture->SetParameter(GL_TEXTURE_MAX_LEVEL, level - 1);
  texture->Unbind();

  shader->End();

  this->device_->RestoreMVP();
  this->device_->Deactivate();

  glPopAttrib();

  ax::CheckErrorsGL("TextureUtil::CreateCustomMipmap");

  return level;
}

void TextureUtil::CreateMaxDepthDerivativeTexture(
    ax::Texture2DPtr position_tex, ax::Texture2DPtr texture) {
  glPushAttrib(GL_ENABLE_BIT);
  glDisable(GL_DEPTH_TEST);

  this->device_->Activate();
  this->device_->SetRenderTarget(texture);
  this->device_->DisableDepthBuffer();
  this->device_->AdjustViewport(texture);
  glClear(GL_COLOR_BUFFER_BIT);

  this->max_depth_derivative_prog_->Begin();

  this->max_depth_derivative_prog_->Set4DMatVar("mvp_mat", this->quad_->mvp());
  this->max_depth_derivative_prog_->SetTextureVar(
      "g_position_buffer", position_tex);
  
  //this->max_depth_derivative_prog_->SetVar("g_zfar", this->camera_.z_far());

  this->max_depth_derivative_prog_->SetVar(
      "g_offset", 1.0f / texture->width());

  this->quad_->Draw(); 
  this->max_depth_derivative_prog_->End();

  this->device_->Deactivate();

  glPopAttrib();
  ax::CheckErrorsGL("TextureUtil::CreateMaxDepthDerivativeTexture");
}

void TextureUtil::CreateMinMaxNormalTexture(
    ax::Texture2DPtr normal_tex, ax::Texture2DPtr texture) {
  glPushAttrib(GL_ENABLE_BIT);
  glDisable(GL_DEPTH_TEST);

  this->device_->Activate();
  this->device_->SetRenderTarget(texture);
  this->device_->DisableDepthBuffer();
  this->device_->AdjustViewport(texture);
  glClear(GL_COLOR_BUFFER_BIT);

  this->min_max_normal_prog_->Begin();

  this->min_max_normal_prog_->Set4DMatVar("mvp_mat", this->quad_->mvp());
  this->min_max_normal_prog_->SetTextureVar("g_normal_tex", normal_tex);
  this->min_max_normal_prog_->SetVar("g_offset", 1.0f / texture->width());

  this->quad_->Draw();

  this->min_max_normal_prog_->End();

  this->device_->Deactivate();

  glPopAttrib();
}

// TODO: make it general
void TextureUtil::Reduce(ax::Texture2DPtr texture, float *ret, int n) { 
  int levels = this->CreateCustomMipmap(this->reduction_prog_, texture, 64); 

  ax::ImagePtr img = texture->GetTextureImage(
      levels - 1, GL_RG, GL_FLOAT, n, 4);

  const float *min_max = (const float*)(img->data());
  float dmin = FLT_MAX;
  float dmax = -dmin;
  for (int i = 0; i < img->width() * img->height(); ++i) {
    if (min_max[i*2] < dmin) dmin = min_max[i*2];
    if (min_max[i*2+1] > dmax) dmax = min_max[i*2+1];
  }
  ret[0] = dmin; ret[1] = dmax;
}

}
