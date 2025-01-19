#pragma once

#include <sstream>
#include <List.hpp>

namespace Tokens {
  enum class TokenType {
    PREPROCESSOR,
    MEMBOX,
    DEFINE,
    IDENTIFIER,
    INTEGER,
    FLOATING,
    OPEN_PAREN,
    CLOSE_PAREN,
    COMMA,
    LESS,
    GREATER,
    OPEN_ANGLE = 9,
    CLOSE_ANGLE = 10
  };

  struct Token {
    TokenType type;
    int line;
    std::string value;
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
}
