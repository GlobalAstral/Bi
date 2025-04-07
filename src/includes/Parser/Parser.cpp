#include <Parser/Parser.hpp>

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
    while ((notFound = hasPeek() && !tryconsume(Tokens::TokenType::close_curly))) {
      parseSingle(nodes);
    }
    if (notFound)
      error({"Missing Token", "Expected closing curly bracket"});
  } else {
    std::string s = peek().toString();
    error({"Syntax Error", Formatting::format("Token %s is nosense", peek().toString().c_str())});
  }
}
