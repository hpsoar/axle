#include "../core/utils.h"

#include <stdio.h>

#if defined(SYS_IS_WINDOWS)
#include <windows.h>
#else
#include <sys/ioctl.h>
#include <unistd.h>
#include <errno.h>
#endif

namespace ax {
int TerminalWidth() {
#ifdef SYS_IS_WINDOWS
  HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
  if (h == INVALID_HANDLE_VALUE || h == NULL) {
    fprintf(stderr, "GetStdHandle() call failed");
    return 80;
  }
  CONSOLE_SCREEN_BUFFER_INFO buffer_info = { 0 };
  GetConsoleScreenBufferInfo(h, &buffer_info);
  return buffer_info.dwSize.X;
#else
  struct winsize w;
  if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) < 0) {
    fprintf(stderr, "Error in ioctl() in TerminalWidth(): %d", errno);
    return 80;
  }
  return w.ws_col;
#endif
}
} // ax
