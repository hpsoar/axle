#ifndef AXLE_CORE_OPTIONS_H
#define AXLE_CORE_OPTIONS_H

#include "axle/core/settings.h"

#include <string>

namespace ax {
class SysOptions {
public:
  static int CoreNum() { 
    assert(s_initialized_);
    return s_cores_num_; 
  }
private:
  static int s_cores_num_;
  static bool s_initialized_;

  friend void InitSysOptions();
};

class UserOptions {
public:
  static const std::string &DefaultMediaPath() { 
    assert(s_initialized_);    
    return s_default_media_path_; 
  }
  static std::string GetFullModelPath(const char *filename);
  static std::string GetFullTexturePath(const char *filename);
  static std::string GetFullEnvMapPath(const char *filename);

private:
  static void set_default_media_path(const char *path);
  static void finalize() { s_initialized_ = true; }
  static bool initialized() { return s_initialized_; }

  static std::string s_default_media_path_;
  static bool s_initialized_;

  friend void LoadUserOptions();
};

void InitOptions();
} // ax

#endif // AXLE_CORE_OPTIONS_H
