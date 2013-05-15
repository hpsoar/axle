#ifndef AXLE_GLOBAL_UTILS_H
#define AXLE_GLOBAL_UTILS_H

#include "axle/core/settings.h"

namespace ax { 
int TerminalWidth();

template<typename T>
inline void SetNULL(T *array[], int count) {
  for (int i = 0; i < count; ++i) array[i] = NULL;
}
template<typename T>
inline void SetNULL(T *&ptr) { ptr = NULL; }

template<typename T>
inline void Delete(T *array[], int count) {
  for (int i = 0; i < count; ++i) {
    delete array[i]; array[i] = NULL;
  }
}

template<typename T>
inline void Delete(T *&ptr) { 
  if (NULL == ptr) return;
  delete ptr; ptr = NULL; 
}

template<typename T>
inline void Delete(T *&ptr, int) {
  if (NULL == ptr) return;
  delete[] ptr; 
  ptr = NULL;
}

template<typename T>
inline void DeleteA(T *&ptr) {
  if (NULL == ptr) return;
  delete[] ptr;
  ptr = NULL;
}
const int s_modulo[4] = { 1, 2, 0, 1 };

} // ax

#endif // AXLE_GLOBAL_UTILS_H
