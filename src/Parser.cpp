#include <Parser.hpp>

Nodes::Expression* Parser::Parser::parseExpr(bool paren) {
  if (tryConsume(Tokens::TokenType::OPEN_PAREN))
    return parseExpr(true);

  Nodes::Expression* expression;
  if (peek()->type == Tokens::TokenType::LITERAL) {
    expression = new Nodes::Expression{Nodes::ExpressionType::literal, {.lit = consume()->value.lit}};
  }

  if (paren)
    tryConsumeError(Tokens::TokenType::CLOSE_PAREN, "Expected ')'");

  return expression;
}

Nodes::Statement* Parser::Parser::parseStmt() {
  
}

Lists::List<Nodes::Statement*> Parser::Parser::parseStmts() {
  Lists::List<Nodes::Statement*> ret{};
  while (hasPeek()) {
    Nodes::Statement* stmt = parseStmt();
    ret.push(stmt);
  }
  return ret;
}
