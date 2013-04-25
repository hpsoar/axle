#ifndef AXLE_CORE_RANDOM_H
#define AXLE_CORE_RANDOM_H

#include "axle/core/settings.h"

#include <stdlib.h>

namespace ax {
inline float Rand(float range) {
  return (float(rand()) / RAND_MAX) * range; 
}

class Random {
public:
  Random() : seed_(0) { }
  Random(int seed) : seed_(seed) {
    srand(seed);
  }
  void set_seed(int seed) { 
    seed_ = seed;
    srand(seed);
  }
  int Uniform(int min_val, int max_val) {
    assert(max_val > min_val);
    int r = rand();
    return min_val + (r % (max_val - min_val));
  }
  float Uniform(float min_val, float max_val) {
    return ax::Rand(max_val - min_val) + min_val;
  }
private:
  int seed_;
};

} // ax

#endif // AXLE_CORE_RANDOM_H
