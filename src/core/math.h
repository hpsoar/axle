#ifndef CHETTAH_CORE_MATH_H
#define CHETTAH_CORE_MATH_H

#include "axle/core/settings.h"

#include <math.h>

namespace ax {
// Constants  
const float kPi = 3.141592653589793238462643383279502884197f;
const float kHalfPi = kPi * .5f;
const float kQuarterPi = kPi * .25f;
const float kTwoPi = kPi * 2;
const float kDegPerRad = 180.f / kPi;
const float kRadPerDeg = kPi / 180.f;

inline float rad2deg(float rad) { return rad * kDegPerRad; }
inline float deg2rad(float deg) { return deg * kRadPerDeg; }
  
template<typename T>
inline T square(T t) { return t * t; }

template<typename T>
inline T clamp(T val, T min, T max) {
  if (val > max) val = max;
  if (val < min) val = min;
  return val;
}

template<typename T, typename T2>
inline T min(T a, T2 b) { return a > b ? b : a; }

template<typename T, typename T2>
inline T max(T a, T2 b) { return a > b ? a : b; } 

inline bool IsPowOf2(int i) { return ((i - 1) & i) == 0; }

inline int Log2(int n) {
  assert(n > 0);
  int i = 0;
  while (n > 1) {
    ++i;
    n >>= 1;
  }
  return i;
}

inline float Log2f(float val) { return log(val) / log(2.0f); }

} // ax

#endif // CHETTAH_CORE_MATH_H

