#ifndef UI_VAR_H
#define UI_VAR_H

#include "../core/settings.h"
#include <map>
#include <sstream>
#include <cctype>
#include <algorithm>

namespace ax {
enum VarActions {
    kVarInc, kVarDec, kVarIncGuarded, kVarDecGuarded, kVarIncLoop, kVarDecLoop, kVarIncStep, kVarDecStep, kVarToggle, kVarSet, kVarReset
  };

class UIVarI {
public:
  class Action {
  public:
    Action() : action_(0), var_(NULL) { }
    Action(int action, ax::UIVarI *var) : action_(action), var_(var) { }

    bool Act() {
      if (this->var_ == NULL) return false;
      return this->var_->PerformAction(this->action_);
    }

    std::string GetDescription() const {
      if (this->var_ == NULL) return std::string("Invalid Action");
      return this->var_->GetActionDescription(this->action_);
    }

    std::string GetVarDescription() const {
      if (this->var_ == NULL) return std::string("Invalid Var");
      return this->var_->GetDescription();
    }

  private:
    int action_;
    UIVarI *var_;
  };

  UIVarI(const std::string &name) : name_(name) { }  

  Action GetAction(int action) { return Action(action, this); }

  const std::string &name() const { return this->name_; }

  void set_name(const std::string &name) {    
    if (name != "") {      
      name_ = name;      
    }
  }

  virtual bool PerformAction(int action) = 0;

  virtual std::string GetActionDescription(int action) const = 0;  

  virtual std::string GetDescription() const = 0;

protected:
  std::string name_;
};

template<typename T>
class UIVar : public UIVarI {
public:
  UIVar(const std::string &name = "UIVar") : UIVarI(name), val_(), step_(), min_(), max_() { }
  // range: [min, max]
  void Set(T val, T step, T min, T max, const std::string &name = "") {
    val_ = val;
    step_ = step;
    min_ = min;
    max_ = max;
    this->set_name(name);
  }

  virtual bool PerformAction(int action) {
    switch (action) {    
    case kVarInc:
      this->Inc();
      break;
    case kVarDec:
      this->Dec();
      break;
    case kVarIncGuarded:
      this->IncChecked();
      break;
    case kVarDecGuarded:
      this->DecChecked();
      break;
    case kVarIncLoop:
      this->IncLoop();
      break;
    case kVarDecLoop:
      this->DecLoop();
      break;
    case kVarIncStep:
      this->IncStep();
      break;
    case kVarDecStep:
      this->DecStep();
      break;
    default:
      return false;
    }
    return true;
  }  

  virtual std::string GetActionDescription(int action) const {
    std::istringstream iss;
    switch (action) {    
    case kVarInc:
      return this->MakeDescription(1, 0, 0);      
    case kVarDec:
      return this->MakeDescription(0, 0, 0);      
    case kVarIncGuarded:
      return this->MakeDescription(1, 0, 1);
    case kVarDecGuarded:
      return this->MakeDescription(0, 0, 1);
    case kVarIncLoop:
      return this->MakeDescription(1, 0, 2);
    case kVarDecLoop:
      return this->MakeDescription(0, 0, 2);
    case kVarIncStep:
      return this->MakeDescription(1, 1, 0);      
    case kVarDecStep:
      return this->MakeDescription(0, 1, 0);
    default:
      return "Invalid Action";
    }    
  }

  virtual std::string GetDescription() const {
    std::stringstream ss;
    ss << "[" + this->name() +"] : ["
      << "value: " << this->val() 
      << "; min: " << this->min() 
      << "; max: " << this->max()
      << "; step: " << this->step() << "]";
    return ss.str();
  }

  void IncStep() { step_ *= 10.f; }
  void DecStep() { step_ *= 0.1f; }

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
  std::string MakeDescription(int action, int type, int tag) const {
    std::stringstream iss;
    if (action > 0) iss << "increment";
    else iss << "decrement";
    
    if (type > 0) iss << " the step size of";

    iss <<  " [" << this->name() << "]";
    if (type == 0) {
      iss << " by [" << this->step() << "]";
      
      if (tag == 1) iss << ", bound in";
      else if (tag == 2) iss << ", loop in";

      if (tag == 1 || tag == 2) iss << " [" << this->min() << ", " << this->max() << "]";
    }
    return iss.str();
  }
private:
  T val_, min_, max_;
  T step_;
};

template<>
class UIVar<bool> : public UIVarI {
public:
  UIVar(const std::string &name = "bool") : UIVarI(name), val_(false) { }

  bool val() const { return val_; }
  void set_val(bool val) { val_ = val; }
  void set_val(int val) { val_ = static_cast<bool>(val); }
  void Set(bool val, const std::string &name="") {
    this->set_val(val);
    this->set_name(name);
  }

  virtual bool PerformAction(int action) {
    switch (action) {
    case ax::kVarToggle:
      this->toggle();
      break;
    case ax::kVarSet:
      this->set_val(true);
      break;
    case ax::kVarReset:
      this->set_val(false);
      break;
    default:
      return false;
    }     
    return true;
  }

  virtual std::string GetActionDescription(int action) const {
    switch (action) {
    case ax::kVarToggle:
      return std::string("toggle [" + this->name() + "]");
    case ax::kVarSet:
      return std::string("set [" + this->name() + "] to true");
    case ax::kVarReset:
      return std::string("set [" + this->name() + "] to false");
    default:
      return "Invalid Action";
    }    
  }

  virtual std::string GetDescription() const {
    if (this->val()) return "[" + this->name() + "]: true";
    else return "[" + this->name() + "]: false";
  }

  void toggle() { val_ = !val_; }
private:
  bool val_;
};
class KeyVarActions {
public: 
  typedef ax::UIVarI::Action Action;
  typedef std::map<int, Action> ActionMap;

  bool OnKey(int key) {
    auto it = this->am_.find(key);
    if (it != this->am_.end()) return it->second.Act();
    return false;
  }

  void Bind(int key, Action act) { this->am_[key] = act; }

  void PrintHelp() const {
    for (int i = 0; i < 80; ++i) printf("-");
    printf("\n");

    for (auto it : am_) {
      printf("[%c]\t%s\n", it.first, it.second.GetDescription().c_str());
    }
  }

  void PrintState() const {
    for (int i = 0; i < 80; ++i) printf("-");
    printf("\n");
    for (auto it : am_) {
      printf("%s\n", it.second.GetVarDescription().c_str());
    }
  }
private:
  ActionMap am_;
};

} // 

#endif // UI_VAR_H
