#pragma once

#include <iostream>
#include <vector>
#include <Tokenizer/Token.hpp>
#include <Utils/Errors.hpp>
#include <Utils/Map.hpp>
#include <Utils/VectorUtils.hpp>

namespace Preprocessor {

  struct Definition {
    std::vector<std::string> params;
    std::vector<Tokens::Token> content;
  };

  class Preprocessor {
    public:
      Preprocessor(std::vector<Tokens::Token>& toks);
      std::vector<Tokens::Token> preprocess();
    private:
      int preprocessIdentifier(Tokens::Token ident, std::vector<Tokens::Token>& out);

      std::vector<Tokens::Token> tokens{};
      int _peek = 0;
      Map::Map<std::string, Definition> definitions{};

      bool hasPeek(int offset = 0);
      Tokens::Token peek(int offset = 0);
      Tokens::Token consume();
      bool tryconsume(Tokens::TokenType type);
      Tokens::Token tryconsume(Tokens::TokenType type, Errors::CompactError error);
      [[noreturn]] void error(Errors::CompactError error);
  };
}

