#pragma once
#include <iostream>
#include <Utils/Formatting.hpp>

namespace Tokens {
  enum class TokenType {
    open_paren, close_paren, open_curly, close_curly, open_angle, close_angle, open_square, close_square, semicolon, at,
    literal, symbols, identifier,
    Int, Float, Long, Double, Char, Byte, String, Void, Mutable, Unsigned, Struct, Union, Interface,
    Return, Operator, Unary, Binary, Asm, Type, If, Else, While, Do, For, Namespace, Defer,
    preprocessor,
  };

  struct Token {
    TokenType type;
    unsigned int line;
    std::string value;
    std::string toString();
  };

}
