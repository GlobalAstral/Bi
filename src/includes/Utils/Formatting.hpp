#pragma once
#include <iostream>
#include <sstream>
#include <vector>
#include <stdarg.h>

namespace Formatting {
  std::string format(std::string format_, ...) {
    using std::stringstream, std::string, std::vector;
    stringstream ss;
    int formats_used = 0;
    va_list args;
    va_start(args, format_);
    const char* f = format_.c_str();

    while (*f) {
      if (*f == '%') {
        f++;
        if (*f == 'd') {
          ss << va_arg(args, int);
        } else if (*f == 's') {
          ss << va_arg(args, char*);
        }
      } else {
        ss << *f;
      }
      f++;
    }
    va_end(args);
    return ss.str();
  }
}

