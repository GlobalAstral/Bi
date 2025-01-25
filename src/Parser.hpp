#pragma once

#include <iostream>
#include <Tokenizer.hpp>

namespace Parser {
  class Parser {
    public:
      Parser(Lists::List<Tokens::Token*> tokens) {
        this->content = tokens;
      }

      
    private:
      int _peek = 0;
      Lists::List<Tokens::Token*> content;
      bool hasPeek() {
        return (this->_peek >= 0 && this->_peek < this->content.size());
      }
      Tokens::Token* peek() {
        if (!hasPeek()) return new Tokens::Token{Tokens::TokenType::NULL_TOKEN, -1};
        return this->content.at(this->_peek);
      }
      Tokens::Token* consume() {
        Tokens::Token* r = peek();
        this->_peek++;
        return r;
      }
      bool tryConsume(Tokens::TokenType type) {
        if (peek()->type == type) {
          consume();
          return true;
        }
        return false;
      }
      Tokens::Token* tryConsumeError(Tokens::TokenType type, std::string error) {
        if (peek()->type == type)
          return consume();
        Errors::error(error);
      }
  };
}

