#pragma once

#include <iostream>
#include <Utils/Errors.hpp>
#include <Utils/StringUtils.hpp>
#include <Utils/Constants.hpp>

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
  };

  Literal& parseLiteral(std::string value);
}

