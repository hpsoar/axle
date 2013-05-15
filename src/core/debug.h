#ifndef AXLE_CORE_DEBUG_H
#define AXLE_CORE_DEBUG_H

#include "axle/core/settings.h"
#include "axle/core/macros.h"

#include <time.h>
#include <stdio.h>
#include <stdarg.h>

namespace ax {
class Logger {
public:
  class Stream {
  public:
    virtual void Print(const char *msg) = 0;
  };
  typedef std::tr1::shared_ptr<Stream> StreamPtr;

  static void SetStream(const StreamPtr &ptr) { s_ = ptr; }

  static void Debug(const char *format, ...) {
#ifndef NDEBUG
    va_list args;
    va_start(args, format);
    ProcessMessage(format, args);
    va_end(args);
#endif
  }
  static void Log(const char *format, ...) {
    va_list args;
    va_start(args, format);
    ProcessMessage(format, args);
    va_end(args);
  }
private:
  static void ProcessMessage(const char *format, va_list args);

private:
  static StreamPtr s_;
};

class StdStream : public Logger::Stream {
public:
  ~StdStream() { 
    fclose(fp_); 
  }

  void Print(const char *msg) {
    fprintf(fp_, "%s\n", msg);
    fflush(fp_);
  }

  static Logger::StreamPtr Create(FILE *fp) {
    return Logger::StreamPtr(new StdStream(fp));
  }

  static Logger::StreamPtr Create(const char *file) {
    FILE *fp = fopen(file, "a+");
    return Logger::StreamPtr(new StdStream(fp));
  }

private:
  StdStream(FILE *fp) : fp_(fp) { }
  
  FILE *fp_;

  DISABLE_COPY_AND_ASSIGN(StdStream);
};

} // ax

#endif // AXLE_CORE_DEBUG_H
