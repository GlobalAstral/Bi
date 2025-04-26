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
    ident = applyNamespaces(ident);
    Nodes::Type* tp = parseType();
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
  } else if (tryconsume({Tokens::TokenType::ellipsis})) {
    nodes.push_back({Nodes::NodeType::pass});
  }
  
  else {
    error({"Syntax Error", Formatting::format("Token %s is nosense", peek().toString().c_str())});
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
    *ret = tryconsume({Tokens::TokenType::identifier}, {"Missing Token", "Expected Identifier after dot for namespace"});
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

Nodes::Type* Parser::Parser::parseType() {
  Nodes::Type* t = new Nodes::Type{};
  if (peek().type == Tokens::TokenType::symbols && consume().value == "*") {
    Nodes::Type* temp = parseType();
    *t = {Nodes::Type::Builtins::Pointer, temp};
  //TODO ARRAY WITH EXPRESSION
  } else if (tryconsume({Tokens::TokenType::Int})) {
    *t = {Nodes::Type::Builtins::Int};
  } else if (tryconsume({Tokens::TokenType::Float})) {
    *t = {Nodes::Type::Builtins::Float};
  } else if (tryconsume({Tokens::TokenType::Long})) {
    *t = {Nodes::Type::Builtins::Long};
  } else if (tryconsume({Tokens::TokenType::Double})) {
    *t = {Nodes::Type::Builtins::Double};
  } else if (tryconsume({Tokens::TokenType::Char})) {
    *t = {Nodes::Type::Builtins::Char};
  } else if (tryconsume({Tokens::TokenType::Byte})) {
    *t = {Nodes::Type::Builtins::Byte};
  } else if (tryconsume({Tokens::TokenType::String})) {
    *t = {Nodes::Type::Builtins::String};
  } else if (tryconsume({Tokens::TokenType::Void})) {
    *t = {Nodes::Type::Builtins::Void};
  } else if (tryconsume({Tokens::TokenType::Struct})) {

    t->type = Nodes::Type::Builtins::Struct;
    if (peek().type == Tokens::TokenType::symbols && consume().value == ":") {
      bool found = doUntilFind({Tokens::TokenType::open_curly}, [this, &t](){
        Nodes::Type* implType = parseType();
        if (implType->type != Nodes::Type::Builtins::Interface)
          error({"Invalid Type", "Expected Interface to implement in struct"});
        if (peek().type != Tokens::TokenType::comma && peek().type != Tokens::TokenType::open_curly)
          error({"Unexpected Token", Formatting::format("Token '%s' is not comma or opening curly bracket", consume().toString().c_str())});
        tryconsume({Tokens::TokenType::comma});
        t->implementing.push_back(implType);
      });
      if (!found)
        error({"Missing Token", "Expected opening curly bracket after struct"});
    }
    bool found = doUntilFind({Tokens::TokenType::close_curly}, [this, &t](){
      if (tryconsume({Tokens::TokenType::Impl})) {
        Nodes::Method mtd = parseMethodSig();
        if (mtd.content.size() <= 0)
          error({"Syntax Error", "Cannot declare method inside struct. Use an interface and implement it instead"});
        for (Nodes::Type* type : t->implementing) {
          bool found = false;
          for (int i = 0; i < type->methods.size(); i++) {
            if (type->methods[i] == mtd) {
              found = true;
              type->methods[i].content = mtd.content;
            }
          }
          if (!found)
            error({"Internal Error", "Cannot find method in implementations for struct"});
        }
      } else {
        Nodes::Type* tp = parseType();
        Tokens::Token ident = tryconsume({Tokens::TokenType::identifier}, {"Missing Token", "Expected Identifier"});
        tryconsume({Tokens::TokenType::semicolon}, {"Missing Token", "Expected semicolon"});
        t->interior[ident.value] = tp;
      }
    });
    if (!found)
      error({"Missing Token", "Expected closing curly bracket"});

  } 
  
  
  else if (tryconsume({Tokens::TokenType::Union})) {
    t->type = Nodes::Type::Builtins::Union;
    
    tryconsume({Tokens::TokenType::open_curly}, {"Missing Token", "Expected opening curly bracket after union"});
    bool found = doUntilFind({Tokens::TokenType::close_curly}, [this, &t](){
      Nodes::Type* tp = parseType();
      Tokens::Token ident = tryconsume({Tokens::TokenType::identifier}, {"Missing Token", "Expected Identifier"});
      tryconsume({Tokens::TokenType::semicolon}, {"Missing Token", "Expected semicolon"});
      t->interior[ident.value] = tp;
    });
    if (!found)
      error({"Missing Token", "Expected closing curly bracket"});
  } else if (tryconsume({Tokens::TokenType::Interface})) {
    t->type = Nodes::Type::Builtins::Interface;
    
    tryconsume({Tokens::TokenType::open_curly}, {"Missing Token", "Expected opening curly bracket after interface"});
    bool found = doUntilFind({Tokens::TokenType::close_curly}, [this, &t](){
      Nodes::Method mtd = parseMethodSig();
      t->methods.push_back(mtd);
    });
    if (!found)
      error({"Missing Token", "Expected closing curly bracket"});
  } else if (peek().type == Tokens::TokenType::identifier) {
    if (!declared_types.contains(peek().value))
      error({"Invalid Type", Formatting::format("Identifier '%s' does not name a type", consume().value.c_str())});
    *t = {Nodes::Type::Builtins::Custom};
    t->alias = consume().value;
  }

  t->mut = tryconsume({Tokens::TokenType::Mutable});
  return t;
}

Nodes::Method Parser::Parser::parseMethodSig() {
  Nodes::Type* retType = parseType();
  Tokens::Token ident = tryconsume({Tokens::TokenType::identifier}, {"Missing Token", "Expected Identifier"});
  ident = applyNamespaces(ident);
  std::vector<Nodes::Variable> params;
  std::vector<Nodes::Node> body;

  if (tryconsume({Tokens::TokenType::open_paren})) {
    bool found = doUntilFind({Tokens::TokenType::close_paren}, [this, &params](){
      Nodes::Type* paramType = parseType();
      Tokens::Token paramIdent = tryconsume({Tokens::TokenType::identifier}, {"Missing Token", "Expected Identifier"});
      if (peek().type != Tokens::TokenType::comma && peek().type != Tokens::TokenType::close_paren)
        error({"Unexpected Token", Formatting::format("Token '%s' is not comma or closing parenthesis", consume().toString().c_str())});
      tryconsume({Tokens::TokenType::comma});
      params.push_back(Nodes::Variable{paramIdent.value, paramType});
    });
    if (!found)
      error({"Missing Token", "Expected closing parenthesis"});
  }
  if (!tryconsume({Tokens::TokenType::semicolon})) {
    tryconsume({Tokens::TokenType::open_curly}, {"Missing Token", "Expected opening curly bracket"});
    bool found = doUntilFind({Tokens::TokenType::close_curly}, [this, &body](){
      parseSingle(body);
    });
    if (!found)
      error({"Missing Token", "Missing closing curly bracket"});
  }
  return Nodes::Method{retType, ident.value, params, body};
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
