#ifndef AXLE_CORE_MICROS_H
#define AXLE_CORE_MICROS_H

#include "../core/types.h"

#define DISABLE_COPY_AND_ASSIGN(TypeName)  \
  TypeName(const TypeName&);               \
  TypeName &operator=(const TypeName&);

#define DISABLE_INSTANCE(TypeName) \
  TypeName();                      \
  DISABLE_COPY_AND_ASSIGN(TypeName);

#define _2X(a) ((a) + (a))
#define _3X(a) ((a) + (a) + (a))

#define NULL_STR "\0"

inline bool Failed(bool ret) {   
  return !ret; 
}

inline bool Failed(int ret) {   
  return ax::kOK != ret; 
}

template<typename T>
inline bool Failed(T ptr)  {  
  return NULL == ptr; 
}

// note that arg may be a function, it should not appear twice in the macro
#define V_RET(arg)                \
{                                 \
  bool failed = Failed(arg);      \
  assert(!failed);                \
  if (failed) return false;       \
}

// note that arg may be a function, it should not appear twice in the macro
#define RET(arg)                  \
{                                 \
  bool failed = Failed(arg);      \
  assert(!failed);                \
  if (failed) return;             \
} 

#endif // AXLE_CORE_MICROS_H
