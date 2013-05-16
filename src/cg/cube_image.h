#ifndef AXLE_CG_CUBEIMAGE_H
#define AXLE_CG_CUBEIMAGE_H

#include "../core/settings.h"
#include "../core/macros.h"

namespace ax {
/*
 * CubeImage is unduplicable, it can be initialized only once
 * on calling Load after image data is initialized, it does nothing
 * and returns false
 */
class CubeImage {
 public:
  CubeImage() : width_(0), height_(0), format_(0), type_(0) {
    for (int i = 0; i < 6; ++i) data_[i] = NULL;
  }
  ~CubeImage() {
    if (0 == width_ || 0 == height_) return;
    width_ = height_ = format_ = type_ = 0;
    for (int i = 0; i < 6; ++i) delete[] data_[i];
  }
  bool Load(const char *filename);
  int width() const { return width_; }
  int height() const { return height_; }
  int format() const { return format_; }
  int type() const { return type_; }
  const char *data(int i) const { return data_[i]; }

 private:
  DISABLE_COPY_AND_ASSIGN(CubeImage);

 private:
  int width_,height_;
  int format_;
  int type_;
  char *data_[6];
};
} // ax

#endif // AXLE_CG_CUBEIMAGE_H
