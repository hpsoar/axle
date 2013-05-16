#ifndef AXLE_CG_PARSER_H
#define AXLE_CG_PARSER_H

#include "../core.h"
#include "../geom.h"
#include "../cg/color.h"

namespace ax {
const char *StripLeading(const char *str);
const char *StripToken(const char *ptr, char *token);
void ToLower(char *str);
float ParseNumber(const char *str);

inline bool Equals(const char *s1, const char *s2) {
  return strcmp(s1, s2) == 0;
}

inline bool IsBlankOrComment(const char *str) { 
  return str[0] == '\0' || str[0] == '#'; 
}

inline ax::Vector4 ExtractVec4(const char *str) {
  float r, g, b, a;
  sscanf(str, "%f%f%f%f", &r, &g, &b, &a);
  return ax::Vector4(r, g, b, a);
}

inline ax::Vector3 ExtractVec3(const char *str) {
  float r, g, b;
  sscanf(str, "%f%f%f", &r, &g, &b);
  return ax::Vector3(r, g, b);
}

inline Colorf ExtractColorf(const char *str) {
  float r, g, b, a;
  sscanf(str, "%f%f%f%f", &r, &g, &b, &a);
  return Colorf(r, g, b, a);
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

