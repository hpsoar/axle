#ifndef AXLE_CORE_TYPEDEFS_H
#define AXLE_CORE_TYPEDEFS_H

#if defined(__GNUC__)
#include <stdint.h>
#elif (_MSC_VER >= 1600)
#include <stdint.h>
#else
typedef          __int8  int8_t;
typedef unsigned __int8  uint8_t;
typedef          __int16 int16_t;
typedef unsigned __int16 uint16_t;
typedef          __int32 int32_t;
typedef unsigned __int32 uint32_t;
typedef          __int64 int64_t;
typedef unsigned __int64 uint64_t;
#endif

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;
typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef uint8_t byte;

//#define EPSILON 0.001

#ifndef NULL
#define NULL 0
#endif

#endif // AXLE_CORE_TYPEDEFS_H
