#include "axle/cg/render_device_fbo.h"

#include "axle/cg/texture_gl.h"
#include "axle/cg/render_buffer.h"

namespace ax {
GLenum RenderDeviceFBO::s_mrt[8] = {
  GL_COLOR_ATTACHMENT0_EXT,
  GL_COLOR_ATTACHMENT1_EXT,
  GL_COLOR_ATTACHMENT2_EXT,
  GL_COLOR_ATTACHMENT3_EXT,
  GL_COLOR_ATTACHMENT4_EXT,
  GL_COLOR_ATTACHMENT5_EXT,
  GL_COLOR_ATTACHMENT6_EXT,
  GL_COLOR_ATTACHMENT7_EXT
};

void RenderDeviceFBO::SetRenderTarget(const Texture2DPtr texture, int level) {
  fbo_.UnattachAllColorAttachement();
  fbo_.AttachTexture(texture->target(), texture->id(),
                     GL_COLOR_ATTACHMENT0_EXT, level);
  glDrawBuffers(1, &s_mrt[0]);
}

void RenderDeviceFBO::SetRenderTargets(const std::vector<Texture2DPtr> &textures, int level) {
  size_t max_count = FramebufferObject::GetMaxColorAttachments(); 
  size_t target_count = textures.size();
  if (target_count > max_count) {
    Logger::Log("RenderDeviceFBO::SetRenderTargetGL, only %d color attachment \
        is supported", max_count);
    return;
  }
  fbo_.UnattachAllColorAttachement();
  for (size_t i = 0; i < target_count; ++i) {
    fbo_.AttachTexture(textures[i]->target(), textures[i]->id(), 
                       GL_COLOR_ATTACHMENT0_EXT + i, level);
  }
  glDrawBuffers(target_count, &s_mrt[0]);
}

void RenderDeviceFBO::SetRenderTarget(const RenderBuffer *render_buffer) {
  fbo_.AttachRenderBuffer(render_buffer->buffer_id());
}
void RenderDeviceFBO::SetRenderTargets(
    int target_count, const RenderBuffer *const render_buffers[]) {
  int max_count = FramebufferObject::GetMaxColorAttachments(); 
  if (target_count > max_count) {
    Logger::Log("RenderDeviceFBO::SetRenderTargetGL, only %d color attachment \
        is supported", max_count);
    return;
  }
  fbo_.UnattachAllColorAttachement();
  for (int i = 0; i < target_count; ++i) {
    fbo_.AttachRenderBuffer(render_buffers[i]->buffer_id(),
                            GL_COLOR_ATTACHMENT0_EXT + i);
  }
}

void RenderDeviceFBO::SetDepthBuffer(const Texture2DPtr texture) {
  fbo_.AttachTexture(texture->target(), texture->id(), 
                     GL_DEPTH_ATTACHMENT_EXT);
}

void RenderDeviceFBO::SetDepthBuffer(const RenderBuffer *render_buffer) {
  fbo_.AttachRenderBuffer(render_buffer->buffer_id(), GL_DEPTH_ATTACHMENT_EXT);
}

void RenderDeviceFBO::SetStencilBuffer(const Texture2DPtr texture) {
  fbo_.AttachTexture(texture->target(), texture->id(), GL_STENCIL_ATTACHMENT);
}

void RenderDeviceFBO::DisableColorBuffer() {
  fbo_.UnattachAllColorAttachement();
  glDrawBuffer(GL_NONE);
  glReadBuffer(GL_NONE);
}

void RenderDeviceFBO::DisableDepthBuffer() {
  fbo_.Unattach(GL_DEPTH_ATTACHMENT_EXT);
}

void RenderDeviceFBO::DisableStencilBuffer() {
  fbo_.Unattach(GL_STENCIL_ATTACHMENT_EXT);
}

void RenderDeviceFBO::SaveMVP() {
  glGetIntegerv(GL_VIEWPORT, s_viewport);
  // read matrix
}
void RenderDeviceFBO::RestoreMVP() {
  glViewport(s_viewport[0], s_viewport[1], s_viewport[2], s_viewport[3]);
  // restore matrix
}

} // ax

