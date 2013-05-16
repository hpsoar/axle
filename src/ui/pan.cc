#include "../ui/pan.h"
#include <glm/gtc/matrix_transform.hpp>

namespace ax {
ax::Matrix4x4 Pan::mat() const {
  return glm::translate(
      glm::mat4(1.f), glm::vec3(this->translate_x_, 0, this->translate_y_));
}

bool Pan::Update(int x, int y) {
  if (this->is_tracking_) {
    float delta_x_ = float(x - last_x_) / this->width_ * radius_ * 
                     radius_factor_;
    float delta_y_ = -float(y - last_y_) / this->height_ * radius_ * 
                     radius_factor_;
    
    this->translate_x_ += delta_x_;
    this->translate_y_ += delta_y_;

    this->UpdatePos(x, y);
    return true;
  }
  return false;
}
} // ax

