#ifndef AXLE_GRAPHICS_FPS_COUNTER_H
#define AXLE_GRAPHICS_FPS_COUNTER_H

#include "axle/core/settings.h"
#include "axle/core/timer.h"

namespace ax {
class FpsCounter {
public:
  // Initializes the frame rate counter.  The input will be the number
  //    of previous frames that FpsCounter will be averaged over.
  enum AverageType { kOverFrames, kOverTime };
  FpsCounter(int average_type = kOverTime, float average_count = 1.0)
      : average_type_(average_type), average_count_(average_count),
        last_time_(-1.0), frame_count_(0),  last_fps_(0.0) { }

  // Call at the end of a frame.  The value returned is the FpsCounter
  //    (i.e., frames per second) averaged over the last N frames, where 
  //    N is the value specified in the constructor.  If the number of
  //    frames rendered is not yet N
  double Update() {
    if (last_time_ < 0.0) last_time_ = GetTime();
    double current_time = GetTime();
    ++frame_count_;
    if (kOverFrames == average_type_) {
      if (frame_count_ >= average_count_) {
        double elapse = current_time - last_time_;
        last_fps_ = frame_count_ / elapse;
        frame_count_ = 0;
        last_time_ = current_time;
      }
    } else {
      double elapse = current_time - last_time_;
      if (elapse >= average_count_) {
        last_fps_ = frame_count_ / elapse;
        frame_count_ = 0;
        last_time_ = current_time;
      }
    }
    return last_fps_;
  }

  double fps() const { return this->last_fps_; }

private:
  const int average_type_;
  float average_count_;
  int frame_count_;
  double last_time_;
  double last_fps_;
};
} // ax

#endif // AXLE_GRAPHICS_FPS_COUNTER_H
