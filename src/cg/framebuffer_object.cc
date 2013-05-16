#include "../cg/framebuffer_object.h"

#include "../core/debug.h"

namespace ax {

FramebufferObject::FramebufferObject()
  : fbo_id_(GenerateFboId()), saved_fbo_id_(0) {
  // Bind this FBO so that it actually gets created now
  GuardedBind();
  GuardedUnbind();
}

FramebufferObject::~FramebufferObject() {
  glDeleteFramebuffersEXT(1, &fbo_id_);
}

void FramebufferObject::Bind() const {
  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo_id_);
}

void FramebufferObject::Disable() {
  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}

bool FramebufferObject::CheckRedundancy(GLenum attachment, GLuint tex_id,
                                        int mip_level,  int z_slice) {
  if(GetAttachedId(attachment) == tex_id && GetAttachedMipLevel(attachment) &&
     GetAttachedZSlice(attachment) == z_slice) return true;
  return false;
}

void FramebufferObject::AttachTexture(GLenum tex_target, GLuint tex_id, 
                                      GLenum attachment, int mip_level, 
                                      int zslice) {
#ifndef AXLE_DEBUG
  if(CheckRedundancy(attachment, tex_id, mip_level, zslice)) {
    Logger::Log("FramebufferObject::AttachTexture, PEFORMANCE WARNING. \
        Redundant bind of texture (id = %d). \
        HINT: Compile with -DNDEBUG to remove this warning.", tex_id); 
  }
#endif
  FramebufferTextureND(attachment, tex_target, tex_id, mip_level, zslice);
}

void FramebufferObject::AttachTextures(int tex_count, GLenum tex_target[], 
                                       GLuint tex_id[], GLenum attachment[], 
                                       int mip_level[], int zslice[]) {
  for (int i = 0; i < tex_count; ++i) {
    AttachTexture(tex_target[i], tex_id[i], 
                  attachment ? attachment[i] : (GL_COLOR_ATTACHMENT0_EXT + i), 
                  mip_level ? mip_level[i] : 0, zslice ? zslice[i] : 0);
  }
}

void FramebufferObject::AttachRenderBuffer(GLuint buffer_id, 
                                           GLenum attachment) {
#ifndef AXLE_DEBUG
  if(GetAttachedId(attachment) == buffer_id) {
    Logger::Log("FramebufferObject::AttachRenderBuffer, PEFORMANCE WARNING, \
        Redundant bind of Renderbuffer (id = %d). \
        HINT : Compile with -DNDEBUG to remove this warning", buffer_id);
  }
#endif
    glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, attachment, 
                                 GL_RENDERBUFFER_EXT, buffer_id);
}

void FramebufferObject::AttachRenderBuffers(
    int buffer_count, GLuint buffer_id[], GLenum attachment[]) {
  for (int i = 0; i < buffer_count; ++i) {
    AttachRenderBuffer(buffer_id[i], attachment ? attachment[i] : 
        (GL_COLOR_ATTACHMENT0_EXT + i));
  }
}

void FramebufferObject::Unattach(GLenum attachment) {
  GLenum type = GetAttachedType(attachment);
  switch(type) {
  case GL_NONE:
    break;
  case GL_RENDERBUFFER_EXT:
    AttachRenderBuffer(0, attachment);
    break;
  case GL_TEXTURE:
    AttachTexture(GL_TEXTURE_2D, 0, attachment);
    break;
  default:
    Logger::Log("FramebufferObject::unbind_attachment ERROR: Unknown attached \
          resource type");
    break;
  }
}

void FramebufferObject::UnattachAll() {
  this->UnattachAllColorAttachement();
  this->Unattach(GL_DEPTH_ATTACHMENT_EXT);
}

void FramebufferObject::UnattachAllColorAttachement() {
  int numAttachments = GetMaxColorAttachments();
  for (int i = 0; i < numAttachments; ++i) {
    Unattach(GL_COLOR_ATTACHMENT0_EXT + i);
  }
}

GLint FramebufferObject::GetMaxColorAttachments() {
  GLint max_color_attachment = 0;
  glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS_EXT, &max_color_attachment);
  return max_color_attachment;
}

GLuint FramebufferObject::GenerateFboId() {
  GLuint id = 0;
  glGenFramebuffersEXT(1, &id);
  return id;
}

void FramebufferObject::GuardedBind() {
  // Only binds if fbo_id_ is different than the currently bound FBO
  glGetIntegerv(GL_FRAMEBUFFER_BINDING_EXT, &saved_fbo_id_);
  if (fbo_id_ != (GLuint)saved_fbo_id_) {
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo_id_);
  }
}

void FramebufferObject::GuardedUnbind() {
  // Returns FBO binding to the previously enabled FBO
  if (fbo_id_ != (GLuint)saved_fbo_id_) {
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, (GLuint)saved_fbo_id_);
  }
}

void FramebufferObject::FramebufferTextureND(GLenum attachment, 
    GLenum tex_target, GLuint tex_id, int mip_level, int zslice) {
  if (tex_target == GL_TEXTURE_1D) {
    glFramebufferTexture1DEXT(GL_FRAMEBUFFER_EXT, attachment,
                              GL_TEXTURE_1D, tex_id, mip_level);
  } else if (tex_target == GL_TEXTURE_3D) {
    glFramebufferTexture3DEXT(GL_FRAMEBUFFER_EXT, attachment,
                              GL_TEXTURE_3D, tex_id, mip_level, zslice);
  } else { 
    // Default is GL_TEXTURE_2D, GL_TEXTURE_RECTANGLE_ARB, or cube faces
    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, attachment, tex_target, 
                              tex_id, mip_level);
  }
}

bool FramebufferObject::CheckBufferStatus() {
  GuardedBind();
  bool isOK = false;

  GLenum status;                                            
  status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
  switch(status) {                                          
  case GL_FRAMEBUFFER_COMPLETE_EXT: // Everything's OK
    isOK = true;
    break;
  case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT:
    Logger::Log("glift::CheckFramebufferStatus() ERROR: \
        GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT");
    isOK = false;
    break;
  case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT:
    Logger::Log("glift::CheckFramebufferStatus() ERROR: \
        GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT");
    isOK = false;
    break;
  case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
    Logger::Log("glift::CheckFramebufferStatus() ERROR: \
        GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT");
    isOK = false;
    break;
  case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
    Logger::Log("glift::CheckFramebufferStatus() ERROR: \
        GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT");
    isOK = false;
    break;
  case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT:
    Logger::Log("glift::CheckFramebufferStatus() ERROR: \
        GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT");
    isOK = false;
    break;
  case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT:
    Logger::Log("glift::CheckFramebufferStatus() ERROR: \
        GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT");
    isOK = false;
    break;
  case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
    Logger::Log("glift::CheckFramebufferStatus() ERROR: \
        GL_FRAMEBUFFER_UNSUPPORTED_EXT");
    isOK = false;
    break;
  default:
    Logger::Log("glift::CheckFramebufferStatus() ERROR: Unknown ERROR");
    isOK = false;
    break;
  }

  GuardedUnbind();
  return isOK;
}

/// Accessors
GLenum FramebufferObject::GetAttachedType(GLenum attachment) {
  // Returns GL_RENDERBUFFER_EXT or GL_TEXTURE
  GLint type = 0;
  glGetFramebufferAttachmentParameterivEXT(GL_FRAMEBUFFER_EXT, attachment, 
      GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE_EXT, &type);
  return GLenum(type);
}

GLuint FramebufferObject::GetAttachedId(GLenum attachment) {
  GLint id = 0;
  glGetFramebufferAttachmentParameterivEXT(GL_FRAMEBUFFER_EXT, attachment, 
      GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME_EXT, &id);
  return GLuint(id);
}

GLint FramebufferObject::GetAttachedMipLevel(GLenum attachment) {
  GLint level = 0;
  glGetFramebufferAttachmentParameterivEXT(GL_FRAMEBUFFER_EXT, attachment, 
      GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL_EXT, &level);
  return level;
}

GLint FramebufferObject::GetAttachedCubeFace(GLenum attachment) {
  GLint level = 0;
  glGetFramebufferAttachmentParameterivEXT(GL_FRAMEBUFFER_EXT, attachment, 
      GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE_EXT, &level);
  return level;
}

GLint FramebufferObject::GetAttachedZSlice(GLenum attachment) {
  GLint slice = 0;
  glGetFramebufferAttachmentParameterivEXT(GL_FRAMEBUFFER_EXT, attachment, 
      GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_3D_ZOFFSET_EXT, &slice);
  return slice;
}

} // ax

