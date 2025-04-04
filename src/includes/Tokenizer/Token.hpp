#pragma once
#include <iostream>

namespace Tokens {
  enum class TokenType {
    star,       identifier,
    slash,      literal,
                symbols
  };

  struct Token {
    TokenType type;
    std::string value;
    unsigned int line;
  };
}
