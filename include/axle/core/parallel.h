#ifndef AXLE_CORE_PARALLEL_H
#define AXLE_CORE_PARALLEL_H

#include "axle/core/settings.h"
#include "axle/core/typedefs.h"

#if defined(AXLE_IS_APPLE_PPC)
#include <libkern/OSAtomic.h>
#endif

#if defined(SYS_IS_WINDOWS)
#include <windows.h>
#else
#include <pthread.h>
//#include <semaphore.h>
#endif
//#include "axle/core/probes.h"

#if defined(SYS_IS_WINDOWS)
	#if _MSC_VER >= 1300
		extern "C" void _ReadWriteBarrier();
		#pragma intrinsic(_ReadWriteBarrier)
	#else
		#define _ReadWriteBarrier()
	#endif

	typedef volatile LONG AtomicInt32;

	#ifdef AXLE_HAS_64_BIT_ATOMICS
		typedef volatile LONGLONG AtomicInt64;
	#endif // 64-bit
#else // !SYS_IS_WINDOWS
	typedef volatile int32_t AtomicInt32;
	#ifdef AXLE_HAS_64_BIT_ATOMICS
		typedef volatile int64_t AtomicInt64;
	#endif
#endif // SYS_IS_WINDOWS

namespace ax {
int NumSystemCores();
} // ax

#endif // AXLE_CORE_PARALLEL_H

