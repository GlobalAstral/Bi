#pragma once
#include <iostream>
#include <Utils/Formatting.hpp>

namespace Tokens {
  enum class TokenType {
    null = -1,
    open_paren, close_paren, open_curly, close_curly, open_angle, close_angle, open_square, close_square, semicolon, at, dot, comma,
    literal, symbols, identifier,
    ellipsis,
    Int, Float, Long, Double, Char, Byte, String, Void, Mutable, Unsigned, Struct, Union, Interface, As,
    Return, Operator, Unary, Binary, Asm, Type, If, Else, While, Do, For, Namespace, Defer, Impl,
    preprocessor, define, undefine, defined, endif, elseif, include
  };

  struct Token {
    TokenType type;
    unsigned int line;
    std::string value;
    std::string toString();
  };

  Token nullToken();

}
