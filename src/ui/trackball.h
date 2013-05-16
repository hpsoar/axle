#ifndef AXLE_UI_TRACKBALL_H
#define AXLE_UI_TRACKBALL_H

#include "../geom.h"
#include <glm/gtc/quaternion.hpp>

namespace ax {
class Trackball {
public:
  Trackball() : window_width_(0), window_height_(0), offset_(0, 0),
                radius_(1.f), radius_translation_(1.f) {
    this->Reset();
  }  
  
  /*
   * reset transformation state
   */
  void Reset() {
    q_now_ = glm::quat(1, 0, 0, 0);    
    q_old_ = q_now_;
    
    rotation_mat_ = glm::mat4(1.0);
    is_tracking_ = false;    
  }

  void ResizeWindow(int width, int height, float radius = 0.9f) {
    window_width_ = width;
    window_height_ = height;
    radius_ = radius;
    center_ = glm::vec2(width * 0.5f, height * 0.5f);
  }

  void SetOffset(int x, int y) {
    offset_.x = x; offset_.y = y;
  }

  void Capture(int x, int y);
  bool Update(int x, int y);
  void Release() { is_tracking_ = false; }

  const Matrix4x4 &rotation_mat() const {    
    return rotation_mat_;
  }
  //const Matrix4x4 &inv_matrix() const { return inv_matrix_; }
private:
  glm::vec3 ScreenPointTo3DPoint(int x, int y);

  bool IsPointInWindow(int x, int y) {
    return x >= offset_.x && x < offset_.x + window_width_ &&
           y >= offset_.y && y < offset_.y + window_height_;
  }
private:
  bool is_tracking_;
  int window_width_;
  int window_height_;  

  Matrix4x4 rotation_mat_;
  Matrix4x4 translation_mat_;
  Matrix4x4 translation_delta_mat_;
  glm::ivec2 offset_;  
  glm::ivec2 center_;
  float radius_;
  float radius_translation_;

  glm::quat q_old_;
  glm::quat q_now_;

  glm::vec3 current_pt_;
  glm::vec3 old_pt_;
  glm::ivec2 old_mouse_pt_;
};

} // ax

#endif // AXLE_UI_TRACKBALL_H

