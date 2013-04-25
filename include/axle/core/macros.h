#ifndef AXLE_CORE_MICROS_H
#define AXLE_CORE_MICROS_H

#include "axle/core/types.h"

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
  assert(ret);
  return !ret; 
}

inline bool Failed(int ret) { 
  assert(ret == ax::kOK);
  return ax::kOK != ret; 
}

template<typename T>
inline bool Failed(T ptr)  {
  assert(ptr != NULL);
  return NULL == ptr; 
}

// note that arg may be a function, it should not appear twice in the macro
#define V_RET(arg)                \
{                                 \
  if (Failed(arg)) return false;  \
}

#define RET(arg)                  \
{                                 \
  if (Failed(arg)) return;        \
}
    

#endif // AXLE_CORE_MICROS_H
