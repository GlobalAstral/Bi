#pragma once

#include <iostream>
#include <vector>

#include <Tokenizer/Token.hpp>
#include <Parser/Nodes.hpp>
#include <Utils/Errors.hpp>
#include <Utils/Formatting.hpp>
#include <Utils/Processor.hpp>
#include <Parser/Types.hpp>

namespace Parser {
  class Parser : public Processor::Processor<Tokens::Token> {
    public:
      Parser(std::vector<Tokens::Token> toks);
      std::vector<Nodes::Node> parse();
    private:
      void parseSingle(std::vector<Nodes::Node>& nodes);
      Tokens::Token& getIdentNamespaces();
      Tokens::Token& applyNamespaces(Tokens::Token& token);

      Types::Type* parseType();

      Tokens::Token null();
      int getCurrentLine();
      bool equalCriteria(Tokens::Token a, Tokens::Token b);

      std::vector<std::string> namespaces{};
      Map::Map<std::string, Types::Type> declared_types{};
  };
}
