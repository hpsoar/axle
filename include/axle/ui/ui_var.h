#ifndef UI_VAR_H
#define UI_VAR_H

namespace ax {
template<typename T>
class UIVar {
public:
  UIVar() : val_(), step_(), min_(), max_() { }
  // range: [min, max]
  void Set(T val, T step, T min, T max) {
    val_ = val;
    step_ = step;
    min_ = min;
    max_ = max;
  }

  void IncStep() { step_ *= 10; }
  void DecStep() { step_ *= 0.1; }

  void Inc() {
    val_ += step_; 
  }
  void Dec() { 
    val_ -= step_; 
  }

  void IncChecked() {
    this->Inc();
    if (val_ > max_) val_ = max_;
  }

  void DecChecked() {
    this->Dec();
    if (val_ < min_) val_ = min_;
  }

  void IncLoop() {
    this->Inc();
    if (val_ > max_) val_ = min_;
  }
  void DecLoop() {
    this->Dec();
    if (val_ < min_) val_ = max_;
  }

  T range() { return max_ - min_ + 1; }
  
  void set_val(T val) { val_ = val; }
  T val() const { return val_; }

  void set_step(T s) { step_ = step; }
  T step() const { return step_; }

  void set_min(T m) { min_ = m; }
  void set_max(T m) { max_ = m; }  

  T min() const { return min_; }
  T max() const { return max_; }
private:
  T val_, min_, max_;
  T step_;
};

template<>
class UIVar<bool> {
public:
  UIVar() : val_(true) { }

  bool val() const { return val_; }
  void set_val(bool val) { val_ = val; }
  void set_val(int val) { val_ = static_cast<bool>(val); }

  void toggle() { val_ = !val_; }
private:
  bool val_;
};
} // vs

#endif // UI_VAR_H
