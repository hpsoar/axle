#include "axle/core/parallel.h"
#include "axle/core/debug.h"

#if !defined(AXLE_IS_WINDOWS)
#include <unistd.h>
#include <sys/sysctl.h>
#endif

namespace ax {
int NumSystemCores() {
#if defined(AXLE_IS_WINDOWS)
  SYSTEM_INFO sysinfo;
  GetSystemInfo(&sysinfo);
  return sysinfo.dwNumberOfProcessors;
#elif defined(AXLE_IS_LINUX)
  return sysconf(_SC_NPROCESSORS_ONLN);
#else
// mac/bsds
#ifdef AXLE_IS_OPENBSD
  int mib[2] = { CTL_HW, HW_NCPU };
#else
  int mib[2];
  mib[0] = CTL_HW;
  size_t length = 2;
  if (sysctlnametomib("hw.logicalcpu", mib, &length) == -1) {
    Logger::Log("sysctlnametomib() filed.  Guessing 2 CPU cores.");
    return 2;
  }
  assert(length == 2);
#endif
  int nCores = 0;
  size_t size = sizeof(nCores);

  /* get the number of CPUs from the system */
  if (sysctl(mib, 2, &nCores, &size, NULL, 0) == -1) {
    Logger::Log("sysctl() to find number of cores present failed");
    return 2;
  }
  return nCores;
#endif
}
} // ax
