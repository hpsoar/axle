#include "../ui/trackball.h"

#include <glm/gtc/matrix_transform.hpp>

#include <GL/glew.h>

namespace ax {
glm::vec3 Trackball::ScreenPointTo3DPoint(int x, int y) {
  // Scale to screen  
  glm::vec3 pt;
  pt.x = -(x - offset_.x - center_.x) / (radius_ * center_.x);
  pt.y = (y - offset_.y - center_.y) / (radius_ * center_.y);

  pt.z = 0.0f;
  float mag = pt.x * pt.x + pt.y * pt.y;

  if (mag > 1.0f) {
    float scale = 1.0f / sqrtf(mag);
    pt.x *= scale;
    pt.y *= scale;
  }
  else {
    pt.z = sqrtf(1.0f - mag);
  }

  // Return vector
  return pt;
}

glm::quat QuatFromBallPoints(const glm::vec3 &from, const glm::vec3 &to) {
  float w = glm::dot(from, to);
  glm::vec3 part = glm::cross(from, to);
  return glm::quat(w, part.x, part.y, part.z);
}

void Trackball::Capture(int x, int y) {
  if (this->IsPointInWindow(x, y)) {
    is_tracking_ = true;
    q_old_ = q_now_;
    old_pt_ = this->ScreenPointTo3DPoint(x, y);
  }  
}

bool Trackball::Update(int x, int y) {
  if (!is_tracking_) return false;

  current_pt_ = this->ScreenPointTo3DPoint(x, y);
  q_now_ = q_old_ * QuatFromBallPoints(old_pt_, current_pt_);
  rotation_mat_ = glm::mat4_cast(q_now_);
  
  return true;
}

} // ax

