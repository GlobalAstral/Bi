#pragma once

#include <iostream>
#include <Tokenizer.hpp>
#include <Nodes.hpp>
#include <Registers.hpp>

namespace Parser {
  class Parser {
    public:
      Parser(Lists::List<Tokens::Token*> tokens) {
        this->content = tokens;
      }

      Nodes::Expression* parseExpr(bool paren = false);
      Nodes::Statement* parseStmt();
      Nodes::DataType* parseDataType();
      Nodes::Type* parseType();
      Lists::List<Nodes::Statement*> parseStmts();
      bool tryParseDataType();
      bool isType();
      Nodes::Method* parseMethodReference(Tokens::Token* identifier);
      Lists::List<Nodes::Method*> getMethodsWithName(char* name);
    private:
      Lists::List<Nodes::Method*> declaredMethods{[](Nodes::Method* a, Nodes::Method* b) {
        return *a == *b;
      }};
      Lists::List<Nodes::Variable*> vars{[](Nodes::Variable* a, Nodes::Variable* b) {
        return std::string(a->name) == std::string(b->name);
      }};
      Lists::List<Nodes::Type*> declaredTypes{[](Nodes::Type* a, Nodes::Type* b) {
        return *a == *b;
      }};
      Lists::List<Nodes::Operation*> declaredOperations{[](Nodes::Operation* a, Nodes::Operation* b) {
        return *a == *b;
      }};
      bool skipStmt = false;
      int _peek = 0;
      Lists::List<Tokens::Token*> content;
      bool hasPeek() {
        return (this->_peek >= 0 && this->_peek < this->content.size());
      }
      Tokens::Token* peek(int offset = 0) {
        this->_peek += offset;
        if (!hasPeek()) return new Tokens::Token{Tokens::TokenType::NULL_TOKEN, -1};
        this->_peek -= offset;
        return this->content.at(this->_peek + offset);
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
        Errors::error(error, peek()->line);
        return {}; //! to get rid of warning
      }
  };
}

