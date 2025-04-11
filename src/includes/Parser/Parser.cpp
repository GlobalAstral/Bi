#include <Parser/Parser.hpp>
#include "Parser.hpp"

Parser::Parser::Parser(std::vector<Tokens::Token> toks) {
  this->tokens = toks;
}

bool Parser::Parser::hasPeek(int offset) {
  return this->_peek + offset >= 0 && this->_peek + offset < this->tokens.size();
}

Tokens::Token Parser::Parser::peek(int offset) {
  return (hasPeek(offset) ? this->tokens[_peek+offset] : Tokens::nullToken());
}

Tokens::Token Parser::Parser::consume() {
  return (hasPeek() ? this->tokens[_peek++] : Tokens::nullToken());
}

bool Parser::Parser::tryconsume(Tokens::TokenType type) {
  if (hasPeek() && peek().type == type) {
    consume();
    return true;
  }
  return false;
}

Tokens::Token Parser::Parser::tryconsume(Tokens::TokenType type, Errors::CompactError error) {
  if (hasPeek() && peek().type == type) {
    return consume();
  }
  Errors::error(error, peek(-1).line);
}

void Parser::Parser::error(Errors::CompactError error) {
  Errors::error(error, peek(-1).line);
}

std::vector<Nodes::Node> Parser::Parser::parse() {
  std::vector<Nodes::Node> nodes{};

  while (hasPeek()) {
    parseSingle(nodes);
  }

  return nodes;
}



void Parser::Parser::parseSingle(std::vector<Nodes::Node> &nodes) {
  if (tryconsume(Tokens::TokenType::Namespace)) {
    Tokens::Token namesp = tryconsume(Tokens::TokenType::identifier, {"Missing Token", "Expected Identifier"});
    tryconsume(Tokens::TokenType::open_curly, {"Missing Token", "Expected opening curly bracket"});
    this->namespaces.push_back(namesp.value);
    bool notFound = false;
    while (notFound = hasPeek() && !tryconsume(Tokens::TokenType::close_curly)) {
      parseSingle(nodes);
    }
    if (notFound)
      error({"Missing Token", "Expected closing curly bracket"});
    this->namespaces.pop_back();
  } else {
    Tokens::Token temp = this->getIdentNamespaces();
    // error({"Syntax Error", Formatting::format("Token %s is nosense", peek().toString().c_str())});
  }
}

Tokens::Token& Parser::Parser::getIdentNamespaces() {
  using std::stringstream;
  Tokens::Token ident = tryconsume(Tokens::TokenType::identifier, {"Missing Token", "Expected Identifier"});
  Tokens::Token* ret = new Tokens::Token{};
  *ret = ident;
  stringstream buf;
  buf << ret->value;
  while (tryconsume(Tokens::TokenType::dot)) {
    *ret = tryconsume(Tokens::TokenType::identifier, {"Missing Token", "Expected identifier after dot for namespace"});
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
