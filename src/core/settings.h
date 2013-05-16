#ifndef AXLE_CORE_SETTINGS_H
#define AXLE_CORE_SETTINGS_H

#if defined(_WIN32) || defined(_WIN64)
#define SYS_IS_WINDOWS
#elif defined(__linux__)
#define SYS_IS_LINUX
#elif defined(__APPLE__)

#define SYS_IS_APPLE
#if !(defined(__i386) || defined(__amd64__))
#define SYS_IS_APPLE_PPC
#else
#define SYS_IS_APPLE_X86
#endif

#elif defined(__OpenBSD__)
#define SYS_IS_OPENBSD
#endif


#include <float.h>
#ifndef INFINITY
#define INFINITY FLT_MAX
#endif

#ifdef M_PI
#undef M_PI
#endif
#define M_PI       3.14159265358979323846f
#define INV_PI     0.31830988618379067154f
#define INV_TWOPI  0.15915494309189533577f
#define INV_FOURPI 0.07957747154594766788f
#if defined(SYS_IS_WINDOWS)
#define alloca _alloca
#endif
#ifndef AXLE_L1_CACHE_LINE_SIZE
#define AXLE_L1_CACHE_LINE_SIZE 64
#endif
#ifndef AXLE_POINTER_SIZE
#if defined(__amd64__) || defined(_M_X64)
#define AXLE_POINTER_SIZE 8
#elif defined(__i386__) || defined(_M_IX86)
#define AXLE_POINTER_SIZE 4
#endif
#endif
#ifndef AXLE_HAS_64_BIT_ATOMICS
#if (AXLE_POINTER_SIZE == 8)
#define AXLE_HAS_64_BIT_ATOMICS
#endif
#endif // AXLE_HAS_64_BIT_ATOMICS

//#if defined(SYS_IS_WINDOWS)
//#include <windows.h>
//#endif
//
//#ifdef max
//#undef max
//#endif
//#ifdef min
//#undef min
//#endif
//
//#ifdef INT
//#undef INT
//#endif

#ifndef NULL
#define NULL 0
#endif

#include <assert.h>
#include <string>

#if defined(SYS_IS_WINDOWS)
#include <memory>
#else
#include <tr1/memory>
#endif
#if !defined(SYS_IS_APPLE) && !defined(SYS_IS_OPENBSD)
#include <malloc.h>
#endif
#if !defined(SYS_IS_WINDOWS) && !defined(SYS_IS_APPLE) && !defined(SYS_IS_OPENBSD)
#include <alloca.h>
#endif

#pragma warning(disable:4996)

#ifdef __GNUC__
#define DEPRECATED(func) func __attribute__ ((deprecated))
#elif defined(_MSC_VER)
#define DEPRECATED(func) __declspec(deprecated) func
#else
#pragma message("WARNING: You need to implement DEPRECATED for this compiler")
#define DEPRECATED(func) func
#endif

#endif // AXLE_CORE_SETTINGS_H
