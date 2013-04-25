#include "axle/cg/cube_image.h"
#include "axle/core/debug.h"

#include <string.h>
#include <IL/il.h>
#include <IL/ilu.h>

namespace ax {

bool CubeImage::Load(const char *filename) {
  if (0 != width_ || 0 != height_) {
    Logger::Log("'CubeImage::Load', class CubeImage don't allow reload");
    return false;
  }
  ilInit();
  ilOriginFunc(IL_ORIGIN_UPPER_LEFT);
  ilEnable(IL_ORIGIN_SET);
  ILuint image_id = ilGenImage();
  ilBindImage(image_id);
  if (!ilLoadImage(filename)) {
    int err_id = ilGetError();
    Logger::Log("'CubeImage::Load', failed to load file '%s'", filename);
    return false;
  }
  int image_width = ilGetInteger(IL_IMAGE_WIDTH);
  int image_height = ilGetInteger(IL_IMAGE_HEIGHT);
  
  int width = image_width / 3;
  int height = image_height / 4;
  if (width != height) {
    Logger::Log("'CubeImage::Load', only cubic image is allowed!");
    ilDeleteImage(image_id);
    return false;
  }

  //extract the six faces from the image
  int bpp = ilGetInteger(IL_IMAGE_BYTES_PER_PIXEL);
  int depth = ilGetInteger(IL_IMAGE_DEPTH);
  //the value for format and type in DevIL is the same to OpenGL
  int format = ilGetInteger(IL_IMAGE_FORMAT);
  int type = ilGetInteger(IL_IMAGE_TYPE);
  int offset_x[6] = { 2, 0, 1, 1, 1, 1 };
  int offset_y[6] = { 1, 1, 0, 2, 1, 3 };
  int face_size = width * height * bpp;
  for (int i = 0; i < 6; ++i) {
    data_[i] = new char[face_size];
    ilCopyPixels(width * offset_x[i], height * offset_y[i], 0,
                 width, height, depth, format, type, data_[i]);
  }
  int channels = ilGetInteger(IL_IMAGE_CHANNELS);
  ilDeleteImage(image_id);

  //the last face need some special care
  ILuint tempid = ilGenImage();
  ilBindImage(tempid);
  ilTexImage(width, height, depth, channels, format, type, data_[5]);
  iluFlipImage();
  iluMirror();
  memcpy(data_[5], ilGetData(), face_size);
  ilDeleteImage(tempid);

  width_ = width; height_ = height; format_ = format; type_ = type;
  return true;
}
} // ax
