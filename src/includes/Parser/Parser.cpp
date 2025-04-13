#include <Parser/Parser.hpp>
#include "Parser.hpp"

Parser::Parser::Parser(std::vector<Tokens::Token> toks) {
  this->content = toks;
}

std::vector<Nodes::Node> Parser::Parser::parse() {
  std::vector<Nodes::Node> nodes{};

  while (hasPeek()) {
    parseSingle(nodes);
  }

  return nodes;
}

void Parser::Parser::parseSingle(std::vector<Nodes::Node> &nodes) {
  if (tryconsume({Tokens::TokenType::Namespace})) {
    Tokens::Token namesp = tryconsume({Tokens::TokenType::identifier}, {"Missing Token", "Expected Identifier"});
    tryconsume({Tokens::TokenType::open_curly}, {"Missing Token", "Expected opening curly bracket"});
    this->namespaces.push_back(namesp.value);
    bool notFound = false;
    while (notFound = hasPeek() && !tryconsume({Tokens::TokenType::close_curly})) {
      parseSingle(nodes);
    }
    if (notFound)
      error({"Missing Token", "Expected closing curly bracket"});
    this->namespaces.pop_back();
  } else if (tryconsume({Tokens::TokenType::Type})) {
    Tokens::Token ident = tryconsume({Tokens::TokenType::identifier}, {"Missing Token", "Expected Identifier"});
    Types::Type* tp = parseType();
    tryconsume({Tokens::TokenType::semicolon}, {"Missing Token", "Expected Semicolon"});
    if (declared_types.contains(ident.value))
      error({"Internal Error", Formatting::format("Type '%s' already exists", ident.value.c_str())});
    declared_types[ident.value] = *tp;
  } else if (tryconsume({Tokens::TokenType::open_curly})) {
    std::vector<Nodes::Node> scope;
    bool notFound = true;
    while (hasPeek()) {
      if (tryconsume({Tokens::TokenType::close_curly})) {
        notFound = false;
        break;
      }
      parseSingle(scope);
    }
    if (notFound)
      error({"Missing Token", "Expected closing curly bracket"});
    nodes.push_back({Nodes::NodeType::scope, {.scope = new Nodes::Scope{scope}}});
  }
  
  else {
    // error({"Syntax Error", Formatting::format("Token %s is nosense", peek().toString().c_str())});
    parseType();
  }
}

Tokens::Token& Parser::Parser::getIdentNamespaces() {
  using std::stringstream;
  Tokens::Token ident = tryconsume({Tokens::TokenType::identifier}, {"Missing Token", "Expected Identifier"});
  Tokens::Token* ret = new Tokens::Token{};
  *ret = ident;
  stringstream buf;
  buf << ret->value;
  while (tryconsume({Tokens::TokenType::dot})) {
    *ret = tryconsume({Tokens::TokenType::identifier}, {"Missing Token", "Expected identifier after dot for namespace"});
    buf << ":" << ret->value;
  }
  std::string temp = buf.str();
  ret->value = temp;
  return *ret;
}

Tokens::Token& Parser::Parser::applyNamespaces(Tokens::Token& token) {
  using std::stringstream, std::string;

  stringstream ss;
  for (int i = 0; i < namespaces.size(); i++)
    ss << namespaces[i] << ":";
  ss << token.value;
  string temp = ss.str();
  Tokens::Token* ret = new Tokens::Token{};
  *ret = token;
  ret->value = temp;
  return *ret;
}

Types::Type* Parser::Parser::parseType() {
  Types::Type* t = new Types::Type{};
  if (peek().type == Tokens::TokenType::symbols && consume().value == "*") {
    Types::Type* temp = parseType();
    *t = {Types::Type::BuiltinTypes::Pointer, temp};
  //TODO ARRAY WITH EXPRESSION
  } else if (tryconsume({Tokens::TokenType::Int})) {
    *t = {Types::Type::BuiltinTypes::Int};
  } else if (tryconsume({Tokens::TokenType::Float})) {
    *t = {Types::Type::BuiltinTypes::Float};
  } else if (tryconsume({Tokens::TokenType::Long})) {
    *t = {Types::Type::BuiltinTypes::Long};
  } else if (tryconsume({Tokens::TokenType::Double})) {
    *t = {Types::Type::BuiltinTypes::Double};
  } else if (tryconsume({Tokens::TokenType::Char})) {
    *t = {Types::Type::BuiltinTypes::Char};
  } else if (tryconsume({Tokens::TokenType::Byte})) {
    *t = {Types::Type::BuiltinTypes::Byte};
  } else if (tryconsume({Tokens::TokenType::String})) {
    *t = {Types::Type::BuiltinTypes::String};
  } else if (tryconsume({Tokens::TokenType::Struct})) {
    t->type = Types::Type::BuiltinTypes::Struct;

    tryconsume({Tokens::TokenType::open_curly}, {"Missing Token", "Expected opening curly bracket after struct"});
    doUntilFind({Tokens::TokenType::close_curly}, [this, t](){
      Types::Type* tp = parseType();
      Tokens::Token ident = tryconsume({Tokens::TokenType::identifier}, {"Missing Token", "Expected Identifier"});
      tryconsume({Tokens::TokenType::semicolon}, {"Missing Token", "Expected semicolon"});
      t->interior[ident.value] = tp;
    });
  } else if (tryconsume({Tokens::TokenType::Union})) {
    t->type = Types::Type::BuiltinTypes::Union;
    
    tryconsume({Tokens::TokenType::open_curly}, {"Missing Token", "Expected opening curly bracket after union"});
    doUntilFind({Tokens::TokenType::close_curly}, [this, t](){
      Types::Type* tp = parseType();
      Tokens::Token ident = tryconsume({Tokens::TokenType::identifier}, {"Missing Token", "Expected Identifier"});
      tryconsume({Tokens::TokenType::semicolon}, {"Missing Token", "Expected semicolon"});
      t->interior[ident.value] = tp;
    });
  } else if (tryconsume({Tokens::TokenType::Interface})) {
    //TODO
  } else if (peek().type == Tokens::TokenType::identifier) {
    if (!declared_types.contains(peek().value))
      error({"Invalid Type", Formatting::format("Identifier '%s' does not name a type", consume().value.c_str())});
    *t = {Types::Type::BuiltinTypes::Custom};
    t->alias = consume().value;
  }

  t->mut = tryconsume({Tokens::TokenType::Mutable});
  return t;
}

Tokens::Token Parser::Parser::null() {
  return Tokens::nullToken();
}

int Parser::Parser::getCurrentLine() {
  return peek(-1).line;
}

bool Parser::Parser::equalCriteria(Tokens::Token a, Tokens::Token b) {
  return a.type == b.type;
}
