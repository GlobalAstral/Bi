#include <Parser.hpp>

Nodes::Expression* Parser::Parser::parseExpr(bool paren) {
  if (tryConsume(Tokens::TokenType::OPEN_PAREN))
    return parseExpr(true);

  Nodes::Expression* expression;
  if (peek()->type == Tokens::TokenType::LITERAL) {
    expression = new Nodes::Expression{Nodes::ExpressionType::literal, {.literal = {consume()->value.lit}}};
  } else if (peek()->type == Tokens::TokenType::IDENTIFIER) {
    Tokens::Token* t = consume();
    expression = new Nodes::Expression{Nodes::ExpressionType::identifier, {.ident = {t->value.buffer}}};
  }

  if (paren)
    tryConsumeError(Tokens::TokenType::CLOSE_PAREN, "Expected ')'");

  return expression;
}

Nodes::DataType* Parser::Parser::parseDataType() {
  if (tryConsume(Tokens::TokenType::MEMBOX)) {
    return new Nodes::DataType{Nodes::DTypeT::MEMBOX, parseExpr()};
  } else if (tryConsume(Tokens::TokenType::LABEL)) {
    return new Nodes::DataType{Nodes::DTypeT::LABEL};
  } else if (tryConsume(Tokens::TokenType::STRUCT)) {
    Lists::List<Nodes::Variable*> ret{};
    tryConsumeError(Tokens::TokenType::OPEN_BRACKET, "Expected '{'");
    bool notFound = false;
    while ((notFound = !tryConsume(Tokens::TokenType::CLOSE_BRACKET))) {
      Nodes::DataType* dt = parseDataType();
      if (dt->type == Nodes::DTypeT::INVALID) Errors::error("Expected datatype");
      Tokens::Token* ident = tryConsumeError(Tokens::TokenType::IDENTIFIER, "Expected identifier");
      tryConsumeError(Tokens::TokenType::SEMICOLON, "Expected ';'");
      ret.push(new Nodes::Variable{dt, ident->value.buffer});
    }
    if (notFound) Errors::error("Expected '}'");
    return new Nodes::DataType{Nodes::DTypeT::STRUCT, new Nodes::Expression{}, ret};
    
  } else if (tryConsume(Tokens::TokenType::UNION)) {
    Lists::List<Nodes::Variable*> ret{};
    tryConsumeError(Tokens::TokenType::OPEN_BRACKET, "Expected '{'");
    bool notFound = false;
    while ((notFound = !tryConsume(Tokens::TokenType::CLOSE_BRACKET))) {
      Nodes::DataType* dt = parseDataType();
      if (dt->type == Nodes::DTypeT::INVALID) Errors::error("Expected datatype");
      Tokens::Token* ident = tryConsumeError(Tokens::TokenType::IDENTIFIER, "Expected identifier");
      tryConsumeError(Tokens::TokenType::SEMICOLON, "Expected ';'");
      ret.push(new Nodes::Variable{dt, ident->value.buffer});
    }
    if (notFound) Errors::error("Expected '}'");
    return new Nodes::DataType{Nodes::DTypeT::UNION, new Nodes::Expression{}, ret};
  }
  return new Nodes::DataType{Nodes::DTypeT::INVALID};
}

Nodes::Statement* Parser::Parser::parseStmt() {
  if (tryConsume(Tokens::TokenType::OPEN_BRACKET)) {
    Lists::List<Nodes::Statement*> scp{};
    bool notFound = false;
    while ((notFound = !tryConsume(Tokens::TokenType::CLOSE_BRACKET)))
      scp.push(parseStmt());
    if (notFound) Errors::error("Expected '}'");
    Nodes::Scope* s = new Nodes::Scope{};
    return new Nodes::Statement{ Nodes::StatementType::scope, { .scope = s} };
  } else if (tryConsume(Tokens::TokenType::METHOD)) {
    bool pub = tryConsume(Tokens::TokenType::PUBLIC);
    bool inline_ = tryConsume(Tokens::TokenType::INLINE);
    Nodes::DataType* dt = parseDataType();
    if (dt->type == Nodes::DTypeT::INVALID) Errors::error("Expected DataType");
    Tokens::Token* ident = tryConsumeError(Tokens::TokenType::IDENTIFIER, "Expected Identifier");
    Lists::List<Nodes::Variable*>* params = new Lists::List<Nodes::Variable*>{};

    if (tryConsume(Tokens::TokenType::OPEN_PAREN)) {
      bool notClosed = false;
      while ((notClosed = !tryConsume(Tokens::TokenType::CLOSE_PAREN))) {
        Nodes::DataType* varDT = parseDataType();
        if (varDT->type == Nodes::DTypeT::INVALID) Errors::error("Expected DataType");
        Tokens::Token* varIdent = tryConsumeError(Tokens::TokenType::IDENTIFIER, "Expected Identifier");
        tryConsume(Tokens::TokenType::COMMA);
        Nodes::Variable* var = new Nodes::Variable{varDT, varIdent->value.buffer};
        params->push(var);
      }
      if (notClosed) Errors::error("Expected ')'");
    }
    Nodes::Method* mtd = new Nodes::Method{ident->value.buffer, pub, inline_, dt, params};
    bool exists = this->declaredMethods.contains(mtd);

    if (tryConsume(Tokens::TokenType::SEMICOLON)) {
      if (exists) Errors::error("Method already exists");
      this->declaredMethods.push(mtd);
      return new Nodes::Statement{Nodes::StatementType::method, {.method = mtd}};
    }
    if (exists) {
      int index = this->declaredMethods.index(mtd);
      Nodes::Method* existing = this->declaredMethods.at(index);
      if (existing->stmt != NULL) Errors::error("Method already exists");
      this->declaredMethods.pop(index);
    }

    Nodes::Statement* stmt = parseStmt();
    if (stmt->type != Nodes::StatementType::scope) Errors::error("Expected scope");
    mtd->stmt = stmt;
    this->declaredMethods.push(mtd);
    return new Nodes::Statement{Nodes::StatementType::method, {.method = mtd}};
  } else if (peek()->type == Tokens::TokenType::ASM) {
    Tokens::Token* asmCode = consume();
    std::string code = std::string(asmCode->value.buffer);
    //TODO REPLACE @Anything with variable once you have variables
    return new Nodes::Statement{Nodes::StatementType::asm_code, {.asmCode = new Nodes::AssemblyCode{const_cast<char*>(code.c_str())}}};
  }
  return {};
}

Lists::List<Nodes::Statement*> Parser::Parser::parseStmts() {
  Lists::List<Nodes::Statement*> ret{};
  Lists::List<Nodes::Variable*> variables{};
  while (hasPeek()) {
    Nodes::Statement* stmt = parseStmt();
    ret.push(stmt);
  }
  return ret;
}
