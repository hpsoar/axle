#ifndef AXLE_CORE_RANDOM_H
#define AXLE_CORE_RANDOM_H

#include "../core/settings.h"

#include <stdlib.h>
#include <time.h>

namespace ax {
inline void set_seed(int seed) {    
  srand(seed);
}
inline void SetTimeSeed() {
  set_seed(time(NULL));
}

template<typename T>
inline T Rand(T range = 1.0f) {
  return (T(rand()) / RAND_MAX) * range; 
}

inline int Uniform(int min_val, int max_val) {
  assert(max_val > min_val);
  int r = rand();
  return min_val + (r % (max_val - min_val));
}

inline float Uniform(float min_val, float max_val) {
  return Rand(max_val - min_val) + min_val;
}

} // ax

#endif // AXLE_CORE_RANDOM_H
