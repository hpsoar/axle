#ifndef AXLE_GRAPHICS_FRAMEBUFFER_OBJECT_H
#define AXLE_GRAPHICS_FRAMEBUFFER_OBJECT_H

#include "axle/core/settings.h"
#include <GL/glew.h>

namespace ax {
class FramebufferObject {
 public:
  FramebufferObject();
  ~FramebufferObject();

  /// Bind this FBO as current render target
  void Bind() const;

  /// Bind a texture to the "attachment" point of this FBO
  void AttachTexture(GLenum tex_target, GLuint tex_id,
                     GLenum attachment = GL_COLOR_ATTACHMENT0_EXT,
                     int mip_level = 0, int zslice = 0);

  /// Bind an array of textures to multiple "attachment" points of this FBO
  ///  - By default, the first 'numTextures' attachments are used,
  ///    starting with GL_COLOR_ATTACHMENT0_EXT
  void AttachTextures(int tex_count, GLenum tex_target[], 
                      GLuint tex_id[], GLenum attachment[] = NULL,
                      int mip_level[] = NULL, int zslice[] = NULL);

  /// Bind a render buffer to the "attachment" point of this FBO
  void AttachRenderBuffer(GLuint buffId,
                          GLenum attachment = GL_COLOR_ATTACHMENT0_EXT);

  /// Bind an array of render buffers to corresponding "attachment" points
  /// of this FBO.
  /// - By default, the first 'numBuffers' attachments are used,
  ///   starting with GL_COLOR_ATTACHMENT0_EXT
  void AttachRenderBuffers(int buffer_count, GLuint buffer_id[],
                           GLenum attachment[] = NULL);

  /// Free any resource bound to the "attachment" point of this FBO
  void Unattach(GLenum attachment);

  /// Free any resources bound to any attachment points of this FBO
  void UnattachAll();

  void UnattachAllColorAttachement();

  /// BEGIN : Accessors
  /// Is attached type GL_RENDERBUFFER_EXT or GL_TEXTURE?
  GLenum GetAttachedType(GLenum attachment);

  /// What is the Id of Renderbuffer/texture currently 
  /// attached to "attachement?"
  GLuint GetAttachedId(GLenum attachment);

  /// Which mipmap level is currently attached to "attachement?"
  GLint  GetAttachedMipLevel(GLenum attachment);

  /// Which cube face is currently attached to "attachment?"
  GLint  GetAttachedCubeFace(GLenum attachment);

  /// Which z-slice is currently attached to "attachment?"
  GLint  GetAttachedZSlice(GLenum attachment);
  /// END : Accessors


  /// BEGIN : Static methods global to all FBOs
  /// Return number of color attachments permitted
  static int GetMaxColorAttachments();

  /// Disable all FBO rendering and return to traditional,
  /// windowing-system controlled framebuffer
  ///  NOTE:
  ///     This is NOT an "unbind" for this specific FBO, but rather
  ///     disables all FBO rendering. This call is intentionally "static"
  ///     and named "Disable" instead of "Unbind" for this reason. The
  ///     motivation for this strange semantic is performance. Providing
  ///     "Unbind" would likely lead to a large number of unnecessary
  ///     FBO enablings/disabling.
  static void Disable();
  /// END : Static methods global to all FBOs

  bool IsValid() { return CheckBufferStatus(); }

  bool CheckBufferStatus();

protected:
  bool CheckRedundancy(GLenum attachment, GLuint tex_id, 
                       int mip_level, int z_slice);
  void  GuardedBind();
  void  GuardedUnbind();
  void  FramebufferTextureND(GLenum attachment, GLenum tex_target, 
                             GLuint tex_id, int mip_level, int z_slice);
  static GLuint GenerateFboId();
private:
  GLuint fbo_id_;
  GLint  saved_fbo_id_;
};
} // ax

#endif // AXLE_GRAPHICS_FRAMEBUFFER_OBJECT_H

