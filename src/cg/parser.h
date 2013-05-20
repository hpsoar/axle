#ifndef AXLE_CG_PARSER_H
#define AXLE_CG_PARSER_H

#include "../core.h"
#include "../geom.h"
#include "../cg/color.h"

namespace ax {
const char *StripLeading(const char *str);
const char *StripToken(const char *ptr, char *token);
void ToLower(char *str);

inline float ParseNumber(const char *str, float def=1.0f) {
  float s;
  if (sscanf(str, "%f", &s) > 0) return s;
  return def;
}

inline bool Equals(const char *s1, const char *s2) {
  return strcmp(s1, s2) == 0;
}

inline bool IsBlankOrComment(const char *str) { 
  return str[0] == '\0' || str[0] == '#'; 
}

template<typename T>
void FixMissing(T v[], int n, int k) {
  if (k < 1) {
    ax::Logger::Log("please provide enough numbers");
    return;
  }
  for (int i = k; i < n; ++i) {
    v[i] = v[k - 1];
  }
}

inline ax::Vector4 ExtractVec4(const char *str) {
  float v[4];
  int k = sscanf(str, "%f%f%f%f", v, v+1, v+2, v+3);
  ax::FixMissing(v, 4, k);
  return ax::Vector4(v[0], v[1], v[2], v[3]);
}

inline ax::Vector3 ExtractVec3(const char *str) {
  float v[3];
  int k = sscanf(str, "%f%f%f", v, v+1, v+2);
  ax::FixMissing(v, 3, k);
  return ax::Vector3(v[0], v[1], v[2]);
}

inline Colorf ExtractColorf(const char *str) {  
  return Colorf(ax::ExtractVec4(str));
}

class LineToken {
public:
  LineToken() { token_[0] = '\0'; }

  bool Equals(const char *str) const {
    return strcmp(token_, str) == 0;
  }

  bool Equals(const std::string &str) const {
    return strcmp(token_, str.c_str()) == 0;
  }

  const char *Extract(const char *ptr) {
    int i = 0;
    for (; ptr[i] != '\0' && !isspace(ptr[i]) && i < 255; ++i)
      token_[i] = ptr[i];
    token_[i] = '\0';
    return ax::StripLeading(ptr + i);
  }  
  const char *ptr() const { return token_; }
private:
  char token_[256];
};

void EatToEndKeyword(FILE *fp, LineToken &token);

inline void Unknown(const char *type, const char *type_name, 
                    const char *obj_type) {
  ax::Logger::Log("Unknown %s '%s' when parsing '%s'!",
                  type, type_name, obj_type);
}

inline void UnknownCommand(const char *cmd, const char *obj_type) {
  ax::Unknown("command", cmd, obj_type);  
}

inline void UnknownKeyword(const char *keyword, const char *obj_type) {
  ax::Unknown("keyword", keyword, obj_type);  
}

inline void Unsupported(const char *obj_type, const char *obj) {
  ax::Logger::Log("Unsupported %s: '%s'", obj_type, obj);
}
} // ax

#endif // AXLE_CG_PARSER_H

