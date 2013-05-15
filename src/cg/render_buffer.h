#ifndef AXLE_GRAPHICS_RENDERBUFFER_H
#define AXLE_GRAPHICS_RENDERBUFFER_H

#include "axle/core/settings.h"
#include <GL/glew.h>

namespace ax {
class RenderBuffer {
 public:
  RenderBuffer(int width, int height, GLint internal_format);
  ~RenderBuffer();
  GLuint buffer_id() const { return buffer_id_; }
  bool is_valid() const { return 0 != buffer_id_; }
  int width() const { return width_; }
  int height() const { return height_; }
 private:
  GLuint buffer_id_;
  int width_, height_;
};
} // ax

#endif // AXLE_GRAPHICS_RENDERBUFFER_H

