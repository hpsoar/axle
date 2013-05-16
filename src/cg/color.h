#ifndef AXLE_CG_COLOR_H
#define AXLE_CG_COLOR_H

namespace ax {
template<typename T>
class Color {
 public:
   Color(T r, T g, T b, T a) : r(r), g(g), b(b), a(a) { }
   
   Color(T r, T g, T b) : r(r), g(g), b(b), a(0) { }

   Color() : r(0), g(0), b(0), a(0) { }

   T *ptr() { return &r; }

   const T *ptr() const { return &r; }

   T r, g, b, a;

   static const Color kBlack;
   static const Color kWhite;
};

template<typename T>
const Color<T> Color<T>::kBlack = Color<T>(0, 0, 0, 1);

template<typename T>
const Color<T> Color<T>::kWhite = Color<T>(1, 1, 1, 1);

typedef Color<float> Colorf;
typedef Color<double> Colord;
}//ax

#endif//AXLE_CG_COLOR_H
