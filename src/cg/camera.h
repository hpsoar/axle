#ifndef AXLE_CG_CAMERA_H
#define AXLE_CG_CAMERA_H

#include <glm/gtc/matrix_transform.hpp>

namespace ax {
class TrackableI {
public:
  TrackableI() : is_captured_(false), w_(1), h_(1), scale_(1.f) { }

  void Capture(int x, int y) {
    is_captured_ = true;
    old_x_ = x;
    old_y_ = y;
  }
  bool Update(int x, int y) {
    if (!this->is_captured_) return false;
    
    float deltax = static_cast<float>(x - old_x_) / w_ * scale_;
    float deltay = static_cast<float>(old_y_ - y) / h_ * scale_;
    this->UpdateObject(deltax, deltay);

    old_x_ = x;
    old_y_ = y;
    return true;
  }

  void Release() { is_captured_ = false; }

  void ResizeWindow(int w, int h) { w_ = w; h_ = h; }
  void Zoom(float f) { scale_ *= f; }
private:
  virtual void UpdateObject(float x, float y) = 0;
protected:
  bool is_captured_;
  int old_x_, old_y_;
  int w_, h_;
  float scale_;
};

class Motion {
public:    
  Motion(const glm::vec3 &pos, const glm::vec3 &target, const glm::vec3 &up)
      : position_(pos), target_(target), up_(up), velocity_(0.5), 
        rotation_speed_(10) {  this->update_(); }
  virtual ~Motion() = 0;

  virtual void Yaw(float angle) = 0; // rotate on up vector
  virtual void Pitch(float angle) = 0; // rotate on right vector
  virtual void Roll(float angle) = 0; // rotate on look vector
  virtual void Strafe(float units) = 0; // move left/right
  virtual void Fly(float units) = 0; // move up/down
  virtual void Walk(float units) = 0; // move forward/backward

  const glm::vec3 &position() const { return position_; }
  const glm::vec3 &target() const { return target_; }
  const glm::vec3 &up() const { return up_; }
  const glm::vec3 &look() const { return look_; }

  void SetPosition(float x, float y, float z) {  SetPosition(glm::vec3(x, y, z)); }
  void SetTarget(float x, float y, float z) { SetTarget(glm::vec3(x, y, z)); }
  void SetUp(float x, float y, float z) { SetUp(glm::vec3(x, y, z)); }
  void SetPosition(const glm::vec3 &p) { set_position(p); update_(); }
  void SetTarget(const glm::vec3 &p) {  set_target(p); update_(); }
  void SetUp(const glm::vec3 &v) { 
    this->set_up(glm::normalize(v));
    this->set_right(glm::cross(this->look(), this->up()));    
  }

  const glm::vec3 &velocity() const { return velocity_; }
  void set_velocity(float x, float y, float z) { 
    velocity_.x = x; velocity_.y = y; velocity_.z = z;
  }
  void set_velocity(const glm::vec3 &v) { velocity_ = v; }

  void inc_velocity(float x, float y, float z) {
    velocity_.x += x; velocity_.y += y; velocity_.z += z;
  }
  void set_rotation_speed(float f) { rotation_speed_ = f; }
  void inc_rotation_speed(float f) { rotation_speed_ += f; }

protected:  
  /* the following three methods simply rotates the camera, and updates
   * direction information, an update of other information is needed to keep 
   * the camera work. and this update is expected to be done by derived 
   * classes according to their need
   */
  void yaw_(float angle);
  void roll_(float angle);
  void pitch_(float angle);
  /* the following three methods simply moves the camera,
   * an update of other information is needed to keep the camera work
   * and this update is for the derived class to do accordingly
   */
  void walk_(float units);
  void fly_(float units);
  void strafe_(float units);
  void set_position(const glm::vec3 &p) { position_ = p; }
  void set_target(const glm::vec3 &p) { target_ = p; }
  void set_up(const glm::vec3 &v) { up_ = v; }
  void set_right(const glm::vec3 &v) { right_ = v; }
  const glm::vec3 &right() const { return right_; }  
  void set_look(const glm::vec3 &v) { look_ = v; }
private:
  void update_() {
    look_ = glm::normalize(target_ - position_);
    right_ = glm::cross(look_, up_);
  }
private:
  float rotation_speed_;
  glm::vec3 velocity_;
  glm::vec3 right_, up_, look_;
  glm::vec3 position_, target_;
};

class FirstPersonMotion : public Motion {
public:
  virtual void Yaw(float angle);
  virtual void Pitch(float angle);
  virtual void Roll(float angle);
  virtual void Strafe(float units);
  virtual void Fly(float units);
  virtual void Walk(float units);
};

class SpectatorMotion : public Motion {
public:
  virtual void Strafe(float units);
  virtual void Fly(float units);
  virtual void Walk(float units);
};

class FlyMotion : public Motion {
public:
  FlyMotion(const glm::vec3 &pos,  const glm::vec3 &target, 
            const glm::vec3 &up = glm::vec3(0, 1, 0))
      : Motion(pos, target, up) { }
  virtual void Yaw(float angle);
  virtual void Pitch(float angle);
  virtual void Roll(float angle);
  virtual void Strafe(float units);
  virtual void Fly(float units);
  virtual void Walk(float units);
private:
  void update_(void);
};

class OrbitMotion : public Motion {
public:
  OrbitMotion(const glm::vec3 &pos, 
              const glm::vec3 &target = glm::vec3(0, 0, 0), 
              const glm::vec3 &up = glm::vec3(0, 1, 0))
      : Motion(pos, target, up), min_dist_(0), max_dist_(FLT_MAX) { }
  virtual void Yaw(float angle);
  virtual void Pitch(float angle);
  virtual void Roll(float angle);
  virtual void Walk(float units);
  virtual void Strafe(float units) { }
  virtual void Fly(float units) { }
  float min_dist() const { return min_dist_; }
  float max_dist() const { return max_dist_; }
  void set_min_dist(float f) { min_dist_ = f; }
  void set_max_dist(float f) { max_dist_ = f; }
private:
  void UpdataPosition();
private:
  float min_dist_, max_dist_;
};

template<typename MotionType>
class PerspectiveCameraGL : public MotionType {
public:  
  PerspectiveCameraGL(const glm::vec3 &pos = glm::vec3(0, 0, 1),
                      const glm::vec3 &target = glm::vec3(0, 0, 0),
                      const glm::vec3 &up = glm::vec3(0, 1, 0))
      : MotionType(pos, target, up), fovy_(40.0), aspect_ratio_(1.0), 
        z_near_(0.1f), z_far_(1000.0f) { }
  glm::mat4 ProjMatrix() const {
    return glm::perspective(fovy_, aspect_ratio_, z_near_, z_far_);
  }
  glm::mat4 ViewMatrix() const {    
    return glm::lookAt(this->position(), this->target(), this->up());
  }  
  glm::mat4 ViewProjMatrix() const {   
    return this->ProjMatrix() * this->ViewMatrix();
  }  
  void set_fovy(float val) { fovy_ = val; }
  void set_aspect_ratio(float val) { aspect_ratio_ = val; }
  void set_z_near(float val) {z_near_ = val; }
  void set_z_far(float val) { z_far_ = val; }
  float fovy() const { return fovy_; }
  float aspect_ratio() const { return aspect_ratio_; }
  float z_near() const { return z_near_; }
  float z_far() const { return z_far_; }
  void inc_z_far(float val) { z_far_ += val; }
  void inc_z_near(float val) { z_near_ += val; }
  void inc_fovy(float val) { fovy_ += val; }
private:
  float fovy_, aspect_ratio_, z_near_, z_far_;
};

template<typename MotionType>
class OrthoCameraGL : public MotionType {
public:
  OrthoCameraGL(float left, float right, float bottom, float up, float near, float far);
};

typedef PerspectiveCameraGL<OrbitMotion> OrbitPerspectiveCameraGL;
typedef PerspectiveCameraGL<FlyMotion> FlyPerspectiveCameraGL;

template<typename C>
class CameraPan : public TrackableI {
public:
  CameraPan(C *camera) : camera_(camera) { }
private:
  void UpdateObject(float deltax, float deltay) {
    camera_->Fly(deltay);
    camera_->Strafe(deltax);
  }
private:
  C *camera_;
};

template<typename C>
class CameraRot : public TrackableI {
public:
  CameraRot(C *camera) : camera_(camera) { }

private:
  void UpdateObject(float deltax, float deltay) {
    camera_->Pitch(deltay);
    camera_->Yaw(-deltax);
  }
private:
  C *camera_;
};
} // ax

#endif//AXLE_CG_CAMERA_H
