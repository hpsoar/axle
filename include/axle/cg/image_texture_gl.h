#ifndef AXLE_GRAPHICS_IMAGE_TEXTURE_GL_H
#define AXLE_GRAPHICS_IMAGE_TEXTURE_GL_H

#include "axle/core/settings.h"
#include "axle/core/typedefs.h"
#include <GL/glew.h>

namespace ax {
enum ImageTextureGLOptions {
kNone = 0,
kUseMipmaps = 0x00000001,
kUseDefaultParameters = 0x00000002,
kDepthTexture = 0x00000004
};

class Image;
class ImageTextureGL {
 public:
  ImageTextureGL(uint32 target);
  ImageTextureGL();
  ~ImageTextureGL();

  void Enable() const;
  void Disable() const;
  void Bind() const;
  void Unbind() const;

  bool Initialize(int width, int height, uint32 internal_format, 
                  int opts = kUseDefaultParameters, void *data = NULL);
  bool Initialize(const Image &image, uint32 internal_format,
                  int opts = kUseDefaultParameters);
  
  void SetDefaultParameters();
  void SetParameter(uint32 name, int32 val);
  void SetParameter(uint32 name, int32 *val);
  void SetParameter(uint32 name, float val);
  void SetParameter(uint32 name, float *val);
  void GenerateMipmap();

  void Save(const char* filename, const int level = 0) const;

  uint32 tex_id() const { return tex_id_; }
  uint32 target() const { return target_; }
  bool is_valid() const { return 0 != tex_id_; }
  int width() const { return width_; }
  int height() const { return height_; }
 private:
  bool BeginInitialize();
  void Release();

 private:
  int width_, height_;
  uint32 tex_id_;
  uint32 target_;
};

} // ax

#endif // AXLE_GRAPHICS_IMAGE_TEXTURE_GL_H
