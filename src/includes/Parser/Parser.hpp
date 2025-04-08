#pragma once

#include <iostream>
#include <vector>

#include <Tokenizer/Token.hpp>
#include <Parser/Nodes.hpp>
#include <Utils/Errors.hpp>
#include <Utils/Formatting.hpp>

namespace Parser {
  class Parser {
    public:
      Parser(std::vector<Tokens::Token> toks);
      std::vector<Nodes::Node> parse();
    private:
      void parseSingle(std::vector<Nodes::Node>& nodes);
      Tokens::Token getIdentNamespaces();
      Tokens::Token applyNamespaces(Tokens::Token token);

      int _peek = 0;
      bool hasPeek(int offset = 0);
      Tokens::Token peek(int offset = 0);
      Tokens::Token consume();
      bool tryconsume(Tokens::TokenType type);
      Tokens::Token tryconsume(Tokens::TokenType type, Errors::CompactError error);
      [[noreturn]] void error(Errors::CompactError error);

      std::vector<Tokens::Token> tokens{};
      std::vector<std::string> namespaces{};
  };
}
