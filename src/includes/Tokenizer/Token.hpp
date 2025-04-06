#pragma once
#include <iostream>
#include <Utils/Formatting.hpp>

namespace Tokens {
  enum class TokenType {
    open_paren, close_paren, open_curly, close_curly, semicolon,
    literal, symbols, identifier,
    Int, Float, Long, Double, Char, Byte, String, Void,
    Return,
  };

  struct Token {
    TokenType type;
    unsigned int line;
    std::string value;
    std::string toString();
  };

  
}
