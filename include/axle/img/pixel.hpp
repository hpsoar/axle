#ifndef AXLE_IMG_PIXEL_H
#define AXLE_IMG_PIXEL_H

#include "axle/core/types.h"

namespace ax {

template<typename T>
struct PixelRgb {
public:
  PixelRgb(void) { }
  PixelRgb(T r, T g, T b) : r(r), g(g), b(b) { }

  PixelRgb operator+(const PixelRgb &p) const {
    return PixelRgb(r + p.r, g + p.g, b + p.b);
  }

  PixelRgb &operator+=(const PixelRgb &p) {
    r += p.r; g += p.g; b += p.b;
    return *this;
  }

  PixelRgb &operator-=(const PixelRgb &p) {
    r -= p.r; g -= p.g; b -= p.b;
    return *this;
  }

  PixelRgb operator-(const PixelRgb &p) const {
    return PixelRgb(r - p.r, g - p.g, b - p.b);
  }

  PixelRgb &operator*=(float s) {
    r *= s; g *= s; b *= s;
    return *this;
  }

  PixelRgb &operator/=(float s) {
    if (s == 0.f) return *this;
    s = 1.f / s;
    return *this *= s;
  }

  PixelRgb operator*(float s) const {
    return PixelRgb(r * s, g * s, b * s);
  }

  PixelRgb operator/(float s) const {
    if (s == 0.f) return *this;
    s = 1.f / s;
    return *this * s;
  }

  friend PixelRgb operator*(float f, PixelRgb &p) {
    return p * f;
  }  

  T b, g, r;
};

template<typename T>
struct PixelRgba {
public:
  PixelRgba(void) {}
  PixelRgba(T r, T g, T b, T a) : r(r), g(g), b(b), a(a) { }  

  T b, g, r, a;
};

typedef byte PixelByte;
typedef float PixelFloat;
typedef PixelRgb<byte> PixelRgbByte;
typedef PixelRgba<byte> PixelRgbaByte;
typedef PixelRgb<float> PixelRgbFloat;
typedef PixelRgba<float> PixelRgbaFloat;

} // ax

#endif // AXLE_IMG_PIXEL_H

