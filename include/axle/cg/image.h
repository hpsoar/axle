#ifndef AXLE_GRAPHICS_IMAGE_H
#define AXLE_GRAPHICS_IMAGE_H

#include "axle/core/settings.h"
#include "axle/core/typedefs.h"
#include "axle/core/macros.h"
#include "axle/cg/cg_fwd.h"

namespace ax {
/*
 * Image is unduplicable, it can be initialized only once
 * on calling Load after image data is initialized, it does nothing
 * and returns false
 */
class Image {
 public:
  // this constructor is just for debug
  static ImagePtr Create(int w, int h, int format, int type, char *data) {
    return ImagePtr(new Image(w, h, format, type, data));
  }

  Image() : width_(0), height_(0), format_(0), type_(0), data_(0) { }
  ~Image() { this->Release(); }
  bool Load(const char *filename);
  int width() const { return width_; }
  int height() const { return height_; }
  int format() const { return format_; }
  int type() const { return type_; }
  const char *data() const { return data_; }

 private:
  void Release() {
    if (0 == width_ || 0 == height_ || data_ == NULL) return;
    width_ = height_ = format_ = type_ = 0;
    delete[] data_;
    data_ = NULL;
  }

  Image(int w, int h, int format, int type, char *data) 
      : width_(w), height_(h), format_(format), type_(type), data_(data) { 
  }

  DISABLE_COPY_AND_ASSIGN(Image);

 private:
  int width_,height_;
  int format_;
  int type_;
  char *data_;
};

void SaveImage(const char *filename, const int w, const int h,
               uint32 depth, uint8 n_channels, uint32 format, uint32 type,
               const void *data);
} // ax

#endif // AXLE_GRAPHICS_IMAGE_H
