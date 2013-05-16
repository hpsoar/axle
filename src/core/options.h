#ifndef AXLE_CORE_OPTIONS_H
#define AXLE_CORE_OPTIONS_H

#include "settings.h"

#include <string>
#include <list>

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

#if defined(SYS_IS_WINDOWS)
const std::string kDefaultMediaPath = "E:/workspace/media/";
#elif defined(SYS_IS_APPLE)
const std::string kDefaultMediaPath = "";
#elif 
const std::string kDefaultMediaPath = "/media/E/workspace/media/";
#endif

class PathList {
public:
  std::string GetFullPath(const char *filename);
  FILE *OpenFile(const char *filename);
  void AddPath(const char *path);
private:
  FILE *OpenFile_(const std::string &path);
  std::list<std::string> path_list_;
};

class UserOptions {
public:
  static const std::string &DefaultMediaPath() { 
    assert(s_initialized_);    
    return s_default_media_path_; 
  }
  DEPRECATED(static std::string GetFullModelPath(const char *filename));
  DEPRECATED(static std::string GetFullTexturePath(const char *filename));
  DEPRECATED(static std::string GetFullEnvMapPath(const char *filename));

  static std::string GetFullPath(const char *filename);

private:
  static void LoadDefault();
  static std::string s_default_media_path_;
  static bool s_initialized_;  
};

void InitOptions();
} // ax

#endif // AXLE_CORE_OPTIONS_H

