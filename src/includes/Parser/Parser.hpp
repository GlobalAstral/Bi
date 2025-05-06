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
#include <string.h>
#include <optional>

namespace Parser {
  class Parser : public Processor::Processor<Tokens::Token> {
    public:
      Parser(std::vector<Tokens::Token> toks);
      std::vector<Nodes::Node*> parse();
    private:
      void parseSingle(std::vector<Nodes::Node*>& nodes);
      Nodes::Expression& parseExpr(bool paren = false);
      Tokens::Token& getIdentNamespaces();
      Tokens::Token& applyNamespaces(Tokens::Token& token);

      Nodes::Type* parseType();
      Nodes::Type* convertFromLiteral(Literals::Literal lit);
      Nodes::Method parseMethodSig();
      bool isType();

      std::vector<Nodes::Method*>* getMethodsWithArgs(char* name, std::vector<Nodes::Variable*> params);

      Tokens::Token null();
      int getCurrentLine();
      bool equalCriteria(Tokens::Token a, Tokens::Token b);

      std::vector<std::string> namespaces{};
      Map::Map<std::string, Nodes::Type*> declared_types{};
      std::vector<Nodes::Method*> methods{};
      std::vector<Nodes::Variable*> variables{};
      std::vector<Nodes::Operation*> operations{};
      std::vector<Nodes::Node*> defers{};
      bool inScope = false;
  };

  class AssemblyParser {
    public:
      AssemblyParser(std::vector<Nodes::Variable*>& vars, std::string txt);
      std::optional<std::vector<Nodes::AssemblyToken*>> parseText();
    private:
      std::vector<Nodes::Variable*>* variables;
      std::string text;
  };
}
