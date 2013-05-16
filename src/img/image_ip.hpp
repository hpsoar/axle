#ifndef AXLE_IMG_IMAGE_IP_HPP
#define AXLE_IMG_IMAGE_IP_HPP

#include "../core/settings.h"

namespace ax {
template<typename T>
void Transpose(const T &src, T &dst) {
  assert(src.height() == dst.width() &&
         src.width() == dst.height());

  if (src.height() != dst.width() ||
      src.width() != dst.height()) return;

  for (int r = 0; r < src.height(); ++r) {
    for (int c = 0; c < src.width(); ++c) {
      dst[c][r] = src[r][c];
    }
  }
}
} // ax
#endif // AXLE_IMG_IMAGE_IP_HPP
