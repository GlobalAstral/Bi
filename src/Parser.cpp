#include <Parser.hpp>

Nodes::Expression* Parser::Parser::parseExpr(bool paren) {
  if (tryConsume(Tokens::TokenType::OPEN_PAREN))
    return parseExpr(true);

  Nodes::Expression* expression;
  if (peek()->type == Tokens::TokenType::LITERAL) {
    expression = new Nodes::Expression{Nodes::ExpressionType::literal, {.literal = {consume()->value.lit}}};
  } else if (peek()->type == Tokens::TokenType::IDENTIFIER) {
    Tokens::Token* t = consume();
    expression = new Nodes::Expression{Nodes::ExpressionType::identifier, {.ident = {t->value.identifier}}};
  }

  if (paren)
    tryConsumeError(Tokens::TokenType::CLOSE_PAREN, "Expected ')'");

  return expression;
}

Nodes::DataType* Parser::Parser::parseDataType() {
  if (tryConsume(Tokens::TokenType::MEMBOX)) {
    return new Nodes::DataType{Nodes::DTypeT::MEMBOX, parseExpr()};
  } else if (tryConsume(Tokens::TokenType::LABEL)) {
    Nodes::Expression* e = parseExpr();
    if (e->type != Nodes::ExpressionType::identifier) Errors::error("Expected identifier after label keyword");
    return new Nodes::DataType{Nodes::DTypeT::LABEL, e};
  }
  //TODO UNION AND STRUCT
  return {};
}

Nodes::Statement* Parser::Parser::parseStmt() {
  
  return {};
}

Lists::List<Nodes::Statement*> Parser::Parser::parseStmts() {
  Lists::List<Nodes::Statement*> ret{};
  while (hasPeek()) {
    Nodes::Statement* stmt = parseStmt();
    ret.push(stmt);
  }
  return ret;
}
