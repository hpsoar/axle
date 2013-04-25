#include "axle/cg/image.h"
#include "axle/core/debug.h"

#include <string.h>

#include <IL/il.h>
#include <IL/ilu.h>

namespace ax {
bool Image::Load(const char *filename) {  
  this->Release();  

  ilInit();
  ilOriginFunc(IL_ORIGIN_LOWER_LEFT);
  ilEnable(IL_ORIGIN_SET);

  ILuint image_id = ilGenImage();
  ilBindImage(image_id);
  if (!ilLoadImage(filename)) {
    Logger::Log("'Image::Load', failed to load file \"%s\"", filename);
    return false;
  }
  width_ = ilGetInteger(IL_IMAGE_WIDTH);
  height_ = ilGetInteger(IL_IMAGE_HEIGHT);
  format_ = ilGetInteger(IL_IMAGE_FORMAT);
  type_ = ilGetInteger(IL_IMAGE_TYPE);
  int bpp = ilGetInteger(IL_IMAGE_BYTES_PER_PIXEL);
  int image_size = width_ * height_ * bpp;
  data_ = new char[image_size];
  memcpy(data_, ilGetData(), image_size);
  ilDeleteImage(image_id);
  
  return true;
}

void SaveImage(const char *filename, const int w, const int h,
               uint32 depth, uint8 n_channels, uint32 format, uint32 type,
               const void *data) {
  ilInit();
  ILuint id = ilGenImage();
  ilTexImage(w, h, depth, n_channels, format, type, const_cast<void*>(data));
  ilEnable(IL_FILE_OVERWRITE);
  ilSaveImage(filename);
  int error = ilGetError();
  ilDeleteImage(id);  
}
} // ax
