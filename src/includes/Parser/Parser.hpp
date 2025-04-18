#pragma once

#include <iostream>
#include <vector>

#include <Tokenizer/Token.hpp>
#include <Parser/Nodes.hpp>
#include <Utils/Errors.hpp>
#include <Utils/Formatting.hpp>
#include <Utils/Processor.hpp>
#include <Utils/Map.hpp>
#include <Utils/VectorUtils.hpp>

namespace Parser {
  class Parser : public Processor::Processor<Tokens::Token> {
    public:
      Parser(std::vector<Tokens::Token> toks);
      std::vector<Nodes::Node> parse();
    private:
      void parseSingle(std::vector<Nodes::Node>& nodes);
      Tokens::Token& getIdentNamespaces();
      Tokens::Token& applyNamespaces(Tokens::Token& token);

      Nodes::Type* parseType();
      Nodes::Method parseMethodSig();

      Tokens::Token null();
      int getCurrentLine();
      bool equalCriteria(Tokens::Token a, Tokens::Token b);

      std::vector<std::string> namespaces{};
      Map::Map<std::string, Nodes::Type> declared_types{};
      std::vector<Nodes::Method> methods{};
  };
}
