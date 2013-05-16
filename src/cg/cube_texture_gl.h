#ifndef AXLE_CG_CUBE_TEXTURE_GL_H
#define AXLE_CG_CUBE_TEXTURE_GL_H

#include "../core/settings.h"
#include "../core/macros.h"
#include <GL/glew.h>

namespace ax {

class CubeImage;
class Image;
/*
 * CubeTextureGL is unduplicable, it can be only initialized once
 * the second call to Initialize is useless, and returns false
 */
class CubeTextureGL {
 public:	
  CubeTextureGL(void) : tex_id_(0) { }
  ~CubeTextureGL(void);
  void Bind(void) const;
  void Unbind(void) const;
  bool Initialize(int width, int height, GLint internal_format);
  bool Initialize(const CubeImage &image, GLint internal_format);
  bool Initialize(Image *images[6], GLuint internal_format);
  GLuint tex_id() const { return tex_id_; }

 private:
  void SetDefaultParameters(void);
  bool BeginInitialize(void);
  bool EndInitialize(void);
  void Release(void);
  DISABLE_COPY_AND_ASSIGN(CubeTextureGL);

 private:
  GLuint tex_id_;
};

} // ax

#endif // AXLE_CG_CUBE_TEXTURE_GL_H
