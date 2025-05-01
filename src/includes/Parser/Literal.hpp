#pragma once

#include <iostream>
#include <Utils/Errors.hpp>
#include <Utils/StringUtils.hpp>
#include <Utils/Constants.hpp>
#include <string.h>

namespace Literals {

  enum class LiteralType {
    INT, FLOAT, LONG, DOUBLE, CHAR, STRING
  };

  struct Literal {
    LiteralType type;
    union {
      int i;
      float f;
      long long l;
      double d;
      char c;
      char* s;
    } u;

    bool operator==(Literal a);
    bool operator!=(Literal a);
  };

  Literal& parseLiteral(std::string value);
}

