#pragma once
#include <iostream>
#include <Errors.hpp>
#include <sstream>

namespace Literal {
  enum class LiteralType {
    integer, long_int, 
    floating, double_floating, 
    character, string, 
    binary, hexadecimal
  };
  
  struct Literal {
    LiteralType type;
    union {
      int i;
      long long l;
      float f;
      double d;
      unsigned long long b;
      unsigned long long h;
      char c;
      char* s;
    } u;
    std::string toString();
    bool operator==(Literal a);
  };

  const std::string HEX_PREFIX = "0x";

  Literal parseLiteral(std::string s);
  bool isSuffix(char c);
  bool hasDot(std::string s);
}

