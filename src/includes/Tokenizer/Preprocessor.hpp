#pragma once

#include <iostream>
#include <vector>
#include <Tokenizer/Token.hpp>
#include <Utils/Errors.hpp>
#include <Utils/Map.hpp>
#include <Utils/VectorUtils.hpp>

#include <Utils/Processor.hpp>

namespace Preprocessor {

  struct Definition {
    std::vector<std::string> params;
    std::vector<Tokens::Token> content;
  };

  class Preprocessor : public Processor::Processor<Tokens::Token> {
    public:
      Preprocessor(std::vector<Tokens::Token>& toks);
      std::vector<Tokens::Token> preprocess();
    private:
      int preprocessIdentifier(Tokens::Token ident, std::vector<Tokens::Token>& out);
      bool preprocessBoolean();
      void preprocessSingle(std::vector<Tokens::Token>& ret);

      Tokens::Token null();
      int getCurrentLine();
      bool equalCriteria(Tokens::Token a, Tokens::Token b);

      Map::Map<std::string, Definition> definitions{};
  };
}
