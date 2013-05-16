#include "../core/debug.h"
#include <stdlib.h>

namespace ax {
Logger::StreamPtr Logger::s_ = StdStream::Create(stderr);

void Logger::ProcessMessage(const char *format, va_list args) {
#if !defined(SYS_IS_WINDOWS)
  char *error_buf;
  if (vasprintf(&error_buf, format, args) == -1) {
    s_->Print("vasprintf() unable to allocate memory!\n");
    abort();
  }
#else
  char error_buf[2048];
  vsnprintf_s(error_buf, sizeof(error_buf), _TRUNCATE, format, args);
#endif
  
  s_->Print(error_buf);

#if !defined(SYS_IS_WINDOWS)
  free(error_buf);
#endif
}

} // ax
