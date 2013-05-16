#include "../cg/camera.h"
#include "../geom.h"

#if defined(SYS_IS_APPLE)
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

namespace ax {
//////////////////////////////////////////////////////////////////////////
Motion::~Motion() { }

void Motion::pitch_(float angle) {
  angle *= rotation_speed_;
  glm::mat4 m(1.f);
  m = glm::rotate(m, angle, this->right());
  const glm::vec4 &new_look = m * glm::vec4(this->look(), 1.f);
  this->set_look(glm::vec3(new_look));
  this->set_up(glm::cross(this->right(), this->look())); 
}

void Motion::yaw_(float angle) {
  angle *= rotation_speed_;
  glm::mat4 m(1.f);
  m = glm::rotate(m, angle, this->up());
  const glm::vec4 &new_look = m * glm::vec4(this->look(), 1.f);
  this->set_look(glm::vec3(new_look));
  this->set_right(glm::cross(this->look(), this->up()));  
}

void Motion::roll_(float angle) {
  angle *= rotation_speed_;
  glm::mat4 m(1.f);
  m = glm::rotate(m, angle, this->look());
  const glm::vec4 &new_up = m * glm::vec4(this->up(), 1.f);
  this->set_look(glm::vec3(new_up));
  this->set_right(glm::cross(this->look(), this->up()));
}

void Motion::walk_(float units) {
  units *= velocity_.z;
  this->set_position(this->position() + this->look() * units);
}

void Motion::strafe_(float units) {
  units *= velocity_.x;
  this->set_position(this->position() + this->right() * units);
}
void Motion::fly_(float units) {
  units *= velocity_.y;
  this->set_position(this->position() + this->up() * units);
}

//////////////////////////////////////////////////////////////////////////
void FirstPersonMotion::Pitch(float angle) {

}
void FirstPersonMotion::Yaw(float angle) {

}
void FirstPersonMotion::Roll(float angle) {

}
void FirstPersonMotion::Walk(float units) {

}
void FirstPersonMotion::Strafe(float units) {

}
void FirstPersonMotion::Fly(float units) {
}
//////////////////////////////////////////////////////////////////////////
void SpectatorMotion::Walk(float units) {

}

//////////////////////////////////////////////////////////////////////////
void FlyMotion::Walk(float units) {
  Motion::walk_(units);
  this->update_();
}

void FlyMotion::Strafe(float units) {
  Motion::strafe_(units);
  this->update_();
}
void FlyMotion::Fly(float units) {
  Motion::fly_(units);
  this->update_();
}

void FlyMotion::Roll(float angle) {  Motion::roll_(angle);  }

void FlyMotion::Pitch(float angle) {
  Motion::pitch_(angle);
  this->update_();
}

void FlyMotion::Yaw(float angle) {
  Motion::yaw_(angle);
  this->update_();
}

void FlyMotion::update_() {
  this->set_target(this->position() + this->look());
}

//////////////////////////////////////////////////////////////////////////
void OrbitMotion::Yaw(float angle) {
  Motion::yaw_(angle);
  UpdataPosition();
}

void OrbitMotion::Roll(float angle) {
  Motion::roll_(angle);
  UpdataPosition();
}
void OrbitMotion::Pitch(float angle) {
  Motion::pitch_(angle);
  UpdataPosition();
}

void OrbitMotion::UpdataPosition() {
  float dist = glm::distance(this->target(), this->position());
  this->set_position(this->target() - this->look() * dist);
}

void OrbitMotion::Walk( float units ) {
  units *= this->velocity().z;
  glm::vec3 new_pos = this->position() + this->look() * units;
  glm::vec3 new_look = this->target() - new_pos;
  if (new_look.x * this->look().x < 0 ||
      new_look.y * this->look().y < 0 ||
      new_look.z * this->look().z < 0) return;
  float dist = glm::length(new_look);
  if (dist < min_dist_ || dist > max_dist_) return;
  this->set_position(new_pos);
}

} // ax


