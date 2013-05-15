#ifndef AXLE_TYPES_H
#define AXLE_TYPES_H

namespace ax {
//template<size_t alignment > class AlignedAllocator;

enum ReturnType {
  kOK,
  kFalse,
  kTrue,
  kInvalidArg,
  kFailed,
  kOutOfMemory,
  kInCompatible,
  kReady,
  kInComplete,
  kPathNotExist
};

enum { kMaxPath = 256 };
enum { kMaxLine = 256 };
} // ax

#include "axle/core/typedefs.h"

#endif // AXLE_TYPES_H
