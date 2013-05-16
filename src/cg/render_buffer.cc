#include "../cg/render_buffer.h"

#include "../cg/utils.h"

namespace ax {
RenderBuffer::RenderBuffer(int width, int height, GLint internal_format) 
    : width_(width), height_(height) {
  glGenRenderbuffersEXT(1, &buffer_id_);
  if (0 == buffer_id_) {
    CheckErrorsGL("RenderBuffer::RenderBuffer");
    return;
  }
  glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, buffer_id_);
  glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, internal_format, width, height);
  glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0);
}

RenderBuffer::~RenderBuffer() {
  if (0 != buffer_id_) {
    glDeleteRenderbuffersEXT(1, &buffer_id_);
    buffer_id_ = 0;
  }
}
} // ax
