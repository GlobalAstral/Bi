#pragma once

#include <sstream>
#include <List.hpp>
#include <Errors.hpp>
#include <Literal.hpp>
#include <string.h>
#include <AssemblyTokenizer.hpp>

namespace Tokens {
  enum class TokenType {
    SEMICOLON,
    PREPROCESSOR,
    MEMBOX,
    LABEL,
    STRUCT,
    UNION,
    DEFINE,
    UNDEFINE,
    IDENTIFIER,
    LITERAL,
    OPEN_PAREN,
    CLOSE_PAREN,
    COMMA,
    OPEN_SQUARE,
    CLOSE_SQUARE,
    OPEN_BRACKET,
    CLOSE_BRACKET,
    DEFINED,
    NOT,
    NULL_TOKEN,
    IF,
    METHOD,
    PUBLIC,
    INLINE,
    ASM,
    TYPE,
    OPERATION,
    BITS,
    SIMD,
    SYMBOLS,
    CAST,
    AS,
    INCLUDE,
    EXTERN
  };

  struct Token {
    TokenType type;
    int line;
    union {
      Literal::Literal lit;
      char* buffer;
      Lists::List<Assembly::Token*>* assemblyCode;
    } value;
    std::string toString();
  };

  class Tokenizer {
    public:
      Tokenizer(std::stringstream& s) {
        this->content = s.str();
      }
      Lists::List<Tokens::Token*> tokenize();
    private:
      std::string content;
      int _peek = 0;
      char consume() {
        return content.at(_peek++);
      }
      char peek(int offset = 0) {
        if (this->_peek == this->content.size())
          return '\0';
        char c = content.at(this->_peek + offset);
        return c;
      }
      bool try_consume(char c) {
        if (peek() == c) {
          consume();
          return true;
        }
        return false;
      }
  };

  bool isChar(char c, std::string s);
}
