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

bool Parser::Parser::tryParseDataType() {
  int prev_peek = _peek;
  Nodes::DataType* dt = parseDataType();
  _peek = prev_peek;
  if (dt->type == Nodes::DTypeT::INVALID) {
    return false;
  }
  return true;
}

Nodes::Statement* Parser::Parser::parseStmt(Lists::List<Nodes::Statement*>& ret, Lists::List<Nodes::Variable*>& vars) {
  if (tryConsume(Tokens::TokenType::OPEN_BRACKET)) {
    Lists::List<Nodes::Statement*> scp{};
    bool notFound = false;
    while ((notFound = !tryConsume(Tokens::TokenType::CLOSE_BRACKET)))
      scp.push(parseStmt(ret, vars));
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
      // int stackTop = (params->size()-1)*8 + 16;
      // for (int i = params->size()-1; i >= 0; i--) {
      //   int offset = stackTop - ((params->size()-1) - i)*8;
      //   params->at(i)->location.offset = offset;
      // }
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

    Nodes::Statement* stmt = parseStmt(ret, vars);
    if (stmt->type != Nodes::StatementType::scope) Errors::error("Expected scope");
    mtd->stmt = stmt;
    this->declaredMethods.push(mtd);
    return new Nodes::Statement{Nodes::StatementType::method, {.method = mtd}};
    
  } else if (peek()->type == Tokens::TokenType::ASM) {
    Tokens::Token* asmCode = consume();
    Lists::List<Assembly::Token*>* code = new Lists::List<Assembly::Token*>{};
    *code = asmCode->value.assemblyCode->copy();
    for (int i = 0; i < code->size(); i++) {
      Assembly::Token* token = code->at(i);
      for (int j = 0; j < token->params.size(); j++) {
        char* param = token->params.at(j);
        if (param[0] != '@') continue;
        param = const_cast<char*>(std::string(param).erase(0, 1).c_str());
        if (!vars.contains(new Nodes::Variable{.name = param})) Errors::error("Variable in assembly code does not exist");
        Nodes::Variable* var = vars.at(vars.index(new Nodes::Variable{.name = param}));
        token->params.pop(j);
        if (!var->inStack) {
          token->params.insert(var->location.reg, j);
        } else {
          char offset[255];
          sprintf(offset, "%d", var->location.offset);
          std::string buf = std::string(offset) + "[rbp]";
          char* repl = (char*)malloc(buf.size()*sizeof(char));
          strcpy(repl, buf.c_str());
          token->params.insert(repl, j);
        }
      }
    }
    return new Nodes::Statement{Nodes::StatementType::asm_code, {.asmCode = new Nodes::AssemblyCode{code}}};
  } else if (tryParseDataType()) {
    Nodes::DataType* dt = parseDataType();
    Tokens::Token* identifier = tryConsumeError(Tokens::TokenType::IDENTIFIER, "Expected Identifier");

    Nodes::Variable* var = new Nodes::Variable{dt, identifier->value.buffer, true};
    if (vars.contains(var)) Errors::error("Variable '" + var->toString() + "' already exists");
    Nodes::Statement* decl = new Nodes::Statement{Nodes::StatementType::var_decl, {.var_decl = new Nodes::VariableDeclaration{var}}};
    vars.push(var);
    if (tryConsume(Tokens::TokenType::SEMICOLON)) {
      return decl;
    }
    tryConsumeError(Tokens::TokenType::EQUALS, "Expected ';' or '='");
    Nodes::Expression* ex = parseExpr();
    tryConsumeError(Tokens::TokenType::SEMICOLON, "Expected ';'");
    ret.push(decl);
    return new Nodes::Statement{Nodes::StatementType::var_set, {.var_set = new Nodes::VariableSetting{var, ex}}};
  }
  Errors::error("Invalid Statement");
  return {};
}

Lists::List<Nodes::Statement*> Parser::Parser::parseStmts() {
  Lists::List<Nodes::Statement*> ret{};
  Lists::List<Nodes::Variable*> variables{[](Nodes::Variable* a, Nodes::Variable* b) {
    return std::string(a->name) == std::string(b->name);
  }};

  while (hasPeek()) {
    Nodes::Statement* stmt = parseStmt(ret, variables);
    ret.push(stmt);
  }
  return ret;
}
