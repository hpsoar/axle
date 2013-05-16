#include "../cg/parser.h"

namespace ax {
const char *StripLeading(const char *str) {  
  while (isspace(*str)) ++str;
  return str;
}

const char *StripToken(const char *ptr, char *token) {
  while (*ptr != '\0' && !isspace(*ptr)) *token++ = *ptr++;
  *token = '\0';
  return StripLeading(ptr);
}

void ToLower(char *str) {
  while (*str != '\0') {
    *str = (char)tolower(*str);
    ++str;
  }
}

float ParseNumber(const char *str) {
  float s;
  sscanf(str, "%f", &s);
  return s;
}

void EatToEndKeyword(FILE *fp, LineToken &token) {
  char buf[512];
  while (fgets(buf, 512, fp) != NULL) {
    const char *ptr = ax::StripLeading(buf);
    token.Extract(ptr);
    if (token.Equals("end")) return;
  }
}
} // ax

