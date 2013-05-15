#ifndef AXLE_PAN_H
#define AXLE_PAN_H

#include "axle/geom.h"
  
namespace ax {
class Pan {
public:
  Pan() : is_tracking_(false), radius_(1.f), radius_factor_(1.f) {
    this->Reset();
  }

  void ResizeWindow(int w, int h) { width_ = w; height_ = h; }

  void Reset() { 
    this->translate_x_ = 0.f;
    this->translate_y_ = 0.f;
    this->radius_factor_ = 1.f;
  }

  void Capture(int x, int y) {
    is_tracking_ = true;
    this->UpdatePos(x, y);
  }

  void ZoomRadius(float f) { this->radius_factor_ *= f; }

  void set_radius(float radius) { radius_ = radius; }

  bool Update(int x, int y);

  void Release() {
    is_tracking_ = false;
  }

  ax::Matrix4x4 mat() const;
private:
  void UpdatePos(int x, int y) {
    last_x_ = x; last_y_ = y;
  }
private:
  bool is_tracking_;
  int last_x_, last_y_;
  float translate_x_, translate_y_;
  int width_, height_;
  float radius_;
  float radius_factor_;
};
} // ax

#endif // AXLE_PAN_H
