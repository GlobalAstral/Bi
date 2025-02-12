#include <Parser.hpp>

Lists::List<Nodes::Method*> Parser::Parser::getMethodsWithName(char* identifier) {
  Lists::List<Nodes::Method*> ret{[](Nodes::Method* a, Nodes::Method* b) {return *a == *b;}};
  for (int i = 0; i < this->declaredMethods.size(); i++) {
    if (std::string(this->declaredMethods.at(i)->identifier) == std::string(identifier))
      ret.push(declaredMethods.at(i));
  }
  return ret;
}

Nodes::Method* Parser::Parser::parseMethodReference(Tokens::Token* t) {
  Lists::List<Nodes::Method*> correspondingMethods = getMethodsWithName(t->value.buffer);
  if (correspondingMethods.size() == 1) {
    return correspondingMethods.at(0);
  } else {
    Tokens::Token* angle = tryConsumeError(Tokens::TokenType::SYMBOLS, "Expected Type Specifier");
    if (std::string(angle->value.buffer) != "<") Errors::error("Expected '<'", peek(-1)->line);
    Nodes::Type* returnType = parseType();
    Lists::List<Nodes::Variable*>* params = new Lists::List<Nodes::Variable*>{};

    bool notFound = false;
    while ((notFound = std::string(tryConsumeError(Tokens::TokenType::SYMBOLS, "Expected '>'")->value.buffer) != ">")) {
      tryConsumeError(Tokens::TokenType::COMMA, "Expected comma");
      Nodes::Type* paramType = parseType();
      params->push(new Nodes::Variable{paramType});
    }
    if (notFound) Errors::error("Expected '>'", peek(-1)->line);
    Nodes::Method* mtd = new Nodes::Method{t->value.buffer, true, false, returnType, params};
    if (!correspondingMethods.contains(mtd)) Errors::error("Method doesn't exist", peek(-1)->line);
    return mtd;
  }
}

Nodes::Operation* Parser::Parser::operationOrError(char* identifier) {
  for (int i = 0; i < this->declaredOperations.size(); i++)
    if (std::string(this->declaredOperations.at(i)->identifier) == std::string(identifier))
      return this->declaredOperations.at(i);
  Errors::error("Operation not declared", peek(-1)->line);
  return {};
}

Nodes::Expression* Parser::Parser::parseExpr(bool paren, bool bin) {
  if (tryConsume(Tokens::TokenType::OPEN_PAREN))
    return parseExpr(true);

  Nodes::Expression* expression;
  if (peek()->type == Tokens::TokenType::LITERAL) {
    Literal::Literal lit = consume()->value.lit;
    expression = new Nodes::Expression{Nodes::ExpressionType::literal, Nodes::getLiteralType(lit), {.literal = {lit}}};
  } else if (peek()->type == Tokens::TokenType::IDENTIFIER) {
    Tokens::Token* t = consume();
    if (getMethodsWithName(t->value.buffer).size() > 0) {
      Nodes::Method* mtd = parseMethodReference(t);
      if (tryConsume(Tokens::TokenType::OPEN_PAREN)) {
        Lists::List<Nodes::Expression*>* params = new Lists::List<Nodes::Expression*>{};
        bool notFound = true;
        while (true) {
          Nodes::Expression* expr = parseExpr();
          params->push(expr);
          if (tryConsume(Tokens::TokenType::CLOSE_PAREN)) {
            notFound = false;
            break;
          }
          tryConsumeError(Tokens::TokenType::COMMA, "Comma Expected");
        }
        if (notFound) Errors::error("Expected ')'", peek(-1)->line);
        if (params->size() != mtd->params->size()) Errors::error("No definition of method with such parameters", peek(-1)->line);
        return new Nodes::Expression{Nodes::ExpressionType::method_call, mtd->returnType, {.method_call = {mtd, params}}};
      } else {
        Nodes::DataType* dt = new Nodes::DataType{Nodes::DTypeT::LABEL};
        return new Nodes::Expression{Nodes::ExpressionType::label, new Nodes::Type{const_cast<char*>("0label"), dt, Registers::RegisterType::b64}, {.label = {mtd}}};
      }
    } else {
      if (!this->vars.contains(new Nodes::Variable{.name = t->value.buffer})) 
        Errors::error("Variable does not exists", peek(-1)->line);
      Nodes::Variable* var = this->vars.at(this->vars.index(new Nodes::Variable{.name = t->value.buffer}));
      return new Nodes::Expression{Nodes::ExpressionType::identifier, var->type, {.ident = {var}}};
    }
  }

  if (bin && peek()->type == Tokens::TokenType::SYMBOLS && std::string(peek()->value.buffer) != "<" && std::string(peek()->value.buffer) != ">") {
    Nodes::Operation* op = operationOrError(peek()->value.buffer);
    int left_prec = op->precedence;
    while (peek()->type == Tokens::TokenType::SYMBOLS) {
      Nodes::Expression* left = expression;
      Nodes::Operation* op1 = operationOrError(consume()->value.buffer);
      Nodes::Expression* right = parseExpr();
      int right_prec = (right->type == Nodes::ExpressionType::binary) ? right->u.bin.operation->precedence : op1->precedence;
      Nodes::Expression* expr = new Nodes::Expression{Nodes::ExpressionType::binary, op1->returnType, {.bin = {left, right, op1}}};
      if (right_prec > left_prec) {
        expr->u.bin.right = left;
        expr->u.bin.left = right;
      } else {
        expr->u.bin.left = left;
        expr->u.bin.right = right;
      }
      expression = expr;
      bool cond = *(expression->u.bin.left->retType->dt) == *(expression->u.bin.operation->lType->type->dt);
      cond = cond && *(expression->u.bin.right->retType->dt) == *(expression->u.bin.operation->rType->type->dt);
      if (!cond) Errors::error("Operation type mismatch", peek(-1)->line);
      left_prec = right_prec;
    }
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
      Nodes::Type* dt = parseType();
      Tokens::Token* ident = tryConsumeError(Tokens::TokenType::IDENTIFIER, "Expected identifier");
      tryConsumeError(Tokens::TokenType::SEMICOLON, "Expected ';'");
      ret.push(new Nodes::Variable{dt, ident->value.buffer});
    }
    if (notFound) Errors::error("Expected '}'", peek(-1)->line);
    return new Nodes::DataType{Nodes::DTypeT::STRUCT, new Nodes::Expression{}, ret};
    
  } else if (tryConsume(Tokens::TokenType::UNION)) {
    Lists::List<Nodes::Variable*> ret{};
    tryConsumeError(Tokens::TokenType::OPEN_BRACKET, "Expected '{'");
    bool notFound = false;
    while ((notFound = !tryConsume(Tokens::TokenType::CLOSE_BRACKET))) {
      Nodes::Type* dt = parseType();

      Tokens::Token* ident = tryConsumeError(Tokens::TokenType::IDENTIFIER, "Expected identifier");
      tryConsumeError(Tokens::TokenType::SEMICOLON, "Expected ';'");
      ret.push(new Nodes::Variable{dt, ident->value.buffer});
    }
    if (notFound) Errors::error("Expected '}'", peek(-1)->line);
    return new Nodes::DataType{Nodes::DTypeT::UNION, new Nodes::Expression{}, ret};
  } else if (peek()->type == Tokens::TokenType::IDENTIFIER) {
    Tokens::Token* ident = peek();
    if (!this->declaredTypes.contains(new Nodes::Type{ident->value.buffer})) 
      return new Nodes::DataType{Nodes::DTypeT::INVALID};
    return this->declaredTypes.at(this->declaredTypes.index(new Nodes::Type{ident->value.buffer}))->dt;
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

Nodes::Type* Parser::Parser::parseType() {
  if (peek()->type != Tokens::TokenType::IDENTIFIER) 
    Errors::error("Expected declared Type", peek(-1)->line);
  Tokens::Token* ident = consume();
  if (!this->declaredTypes.contains(new Nodes::Type{ident->value.buffer})) 
    Errors::error("Type not declared", peek(-1)->line);
  return this->declaredTypes.at(this->declaredTypes.index(new Nodes::Type{ident->value.buffer}));
}

bool Parser::Parser::isType() {
  return (peek()->type == Tokens::TokenType::IDENTIFIER && this->declaredTypes.contains(new Nodes::Type{peek()->value.buffer}));
}

Nodes::Statement* Parser::Parser::parseStmt() {
  if (tryConsume(Tokens::TokenType::OPEN_BRACKET)) {
    Lists::List<Nodes::Statement*> scp{};
    Lists::List<Nodes::Variable*> old_vars = vars.copy();
    bool notFound = false;
    while ((notFound = !tryConsume(Tokens::TokenType::CLOSE_BRACKET)))
      scp.push(parseStmt());
    if (notFound) Errors::error("Expected '}'", peek(-1)->line);
    this->vars = old_vars;
    Nodes::Scope* s = new Nodes::Scope{};
    return new Nodes::Statement{ Nodes::StatementType::scope, { .scope = s} };

  } else if (tryConsume(Tokens::TokenType::METHOD)) {
    bool pub = tryConsume(Tokens::TokenType::PUBLIC);
    bool inline_ = tryConsume(Tokens::TokenType::INLINE);
    Nodes::Type* dt = parseType();
    Tokens::Token* ident = tryConsumeError(Tokens::TokenType::IDENTIFIER, "Expected Identifier");
    Lists::List<Nodes::Variable*>* params = new Lists::List<Nodes::Variable*>{};

    if (tryConsume(Tokens::TokenType::OPEN_PAREN)) {
      bool notClosed = false;
      while ((notClosed = !tryConsume(Tokens::TokenType::CLOSE_PAREN))) {
        Nodes::Type* varDT = parseType();
        Tokens::Token* varIdent = tryConsumeError(Tokens::TokenType::IDENTIFIER, "Expected Identifier");
        tryConsume(Tokens::TokenType::COMMA);
        Nodes::Variable* var = new Nodes::Variable{varDT, varIdent->value.buffer};
        params->push(var);
      }
      if (notClosed) Errors::error("Expected ')'", peek(-1)->line);
    }

    Nodes::Method* mtd = new Nodes::Method{ident->value.buffer, pub, inline_, dt, params};
    bool exists = this->declaredMethods.contains(mtd);

    if (tryConsume(Tokens::TokenType::SEMICOLON)) {
      if (exists) Errors::error("Method already exists", peek(-1)->line);
      this->declaredMethods.push(mtd);
      return new Nodes::Statement{Nodes::StatementType::method, {.method = mtd}};
    }
    
    if (exists) {
      int index = this->declaredMethods.index(mtd);
      Nodes::Method* existing = this->declaredMethods.at(index);
      if (existing->stmt != NULL) Errors::error("Method already exists", peek(-1)->line);
      this->declaredMethods.pop(index);
    }
    Lists::List<Nodes::Variable*> old_vars = vars.copy();
    for (int i = 0; i < params->size(); i++)
      vars.push(params->at(i));
    Nodes::Statement* stmt = parseStmt();
    this->vars = old_vars;
    if (stmt->type != Nodes::StatementType::scope) Errors::error("Expected scope", peek(-1)->line);
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
        if (!this->vars.contains(new Nodes::Variable{.name = param})) Errors::error("Variable in assembly code does not exist", peek(-1)->line);
        Nodes::Variable* var = this->vars.at(this->vars.index(new Nodes::Variable{.name = param}));
        token->params.pop(j);
        if (!var->inStack) {
          token->params.insert((char*)(var->location.reg), j);
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
  } else if (isType()) {
    Nodes::Type* dt = parseType();
    Tokens::Token* identifier = tryConsumeError(Tokens::TokenType::IDENTIFIER, "Expected Identifier");

    Nodes::Variable* var = new Nodes::Variable{dt, identifier->value.buffer, true};
    if (this->vars.contains(var)) Errors::error("Variable '" + var->toString() + "' already exists", peek(-1)->line);
    Nodes::Statement* decl = new Nodes::Statement{Nodes::StatementType::var_decl, {.var_decl = new Nodes::VariableDeclaration{var}}};
    this->vars.push(var);
    if (tryConsume(Tokens::TokenType::SEMICOLON)) {
      return decl;
    }
    Tokens::Token* eq = tryConsumeError(Tokens::TokenType::SYMBOLS, "Expected ';' or '='");
    if (std::string(eq->value.buffer) != "=") Errors::error("Expected '='", peek(-1)->line);
    Nodes::Expression* ex = parseExpr();
    tryConsumeError(Tokens::TokenType::SEMICOLON, "Expected ';'");
    Nodes::Statement* setting = new Nodes::Statement{Nodes::StatementType::var_set, {.var_set = new Nodes::VariableSetting{var, ex}}};
    return new Nodes::Statement{Nodes::StatementType::var_init, {.var_init = new Nodes::VariableInitialization{decl, setting}}};
  } else if (peek()->type == Tokens::TokenType::IDENTIFIER) {
    Tokens::Token* ident = consume();
    Tokens::Token* eq = tryConsumeError(Tokens::TokenType::SYMBOLS, "Expected '='");
    if (std::string(eq->value.buffer) != "=") Errors::error("Expected '='", peek(-1)->line);
    int index = -1;
    for (int i = 0; i < this->vars.size(); i++) {
      if (std::string(this->vars.at(i)->name) != std::string(ident->value.buffer)) continue;
      index = i;
      break;
    }
    if (index == -1) Errors::error("Variable '" + std::string(ident->value.buffer) + "' does not exist", peek(-1)->line);
    Nodes::Expression* expr = parseExpr();
    Nodes::Variable* var = this->vars.at(index);
    tryConsumeError(Tokens::TokenType::SEMICOLON, "Expected ';'");
    return new Nodes::Statement{Nodes::StatementType::var_set, {.var_set = new Nodes::VariableSetting{var, expr}}};
  
  } else if (tryConsume(Tokens::TokenType::TYPE)) {
    Tokens::Token* ident = tryConsumeError(Tokens::TokenType::IDENTIFIER, "Expected identifier");
    Nodes::DataType* dt = parseDataType();
    if (dt->type == Nodes::DTypeT::INVALID) Errors::error("Expected valid DataType", peek(-1)->line);
    tryConsumeError(Tokens::TokenType::BITS, "Expected bits directive");
    Registers::RegisterType regtype;
    if (tryConsume(Tokens::TokenType::SIMD))
      regtype = Registers::RegisterType::simd;
    else {
      Nodes::Expression* expr = parseExpr();
      if (expr->type != Nodes::ExpressionType::literal) Errors::error("Expected integer literal", peek(-1)->line);
      if (expr->u.literal.lit.type != Literal::LiteralType::integer) Errors::error("Expected integer literal", peek(-1)->line);
      int l = expr->u.literal.lit.u.i;
      switch (l) {
        case 8 :
          regtype = Registers::RegisterType::b8;
          break;
        case 16 :
          regtype = Registers::RegisterType::b16;
          break;
        case 32 :
          regtype = Registers::RegisterType::b32;
          break;
        case 64 :
          regtype = Registers::RegisterType::b64;
          break;
      }
    }
    tryConsumeError(Tokens::TokenType::SEMICOLON, "Expected ';'");
    Nodes::Type* t = new Nodes::Type{ident->value.buffer, dt, regtype};
    this->declaredTypes.push(t);
    this->skipStmt = true;
    return new Nodes::Statement{};

  } else if (tryConsume(Tokens::TokenType::OPERATION)) {
    Tokens::Token* open_angle = tryConsumeError(Tokens::TokenType::SYMBOLS, "Expected '<'");
    if (std::string(open_angle->value.buffer) != "<") Errors::error("Expected '<'", peek(-1)->line);
    Nodes::Type* left = parseType();
    Tokens::Token* left_ident = tryConsumeError(Tokens::TokenType::IDENTIFIER, "Expected Identifier");
    tryConsumeError(Tokens::TokenType::COMMA, "Expected comma");
    Nodes::Type* right = parseType();
    Tokens::Token* right_ident = tryConsumeError(Tokens::TokenType::IDENTIFIER, "Expected Identifier");
    tryConsumeError(Tokens::TokenType::COMMA, "Expected comma");
    Tokens::Token* identifier = tryConsumeError(Tokens::TokenType::SYMBOLS, "Expected symbols");
    char* ident = identifier->value.buffer;
    tryConsumeError(Tokens::TokenType::COMMA, "Expected comma");
    Nodes::Expression* expr = parseExpr();
    if (expr->type != Nodes::ExpressionType::literal) Errors::error("Expected Integer literal");
    if (expr->u.literal.lit.type != Literal::LiteralType::integer) Errors::error("Expected Integer literal");
    int prec = expr->u.literal.lit.u.i;
    Tokens::Token* close_angle = tryConsumeError(Tokens::TokenType::SYMBOLS, "Expected '>'");
    if (std::string(close_angle->value.buffer) != ">") Errors::error("Expected '>'", peek(-1)->line);
    Registers::RegMapping mapping = Registers::getMappings(left->regType);
    Nodes::Variable* l = new Nodes::Variable{left, left_ident->value.buffer, false, {.reg = mapping.A}};
    mapping = Registers::getMappings(right->regType);
    Nodes::Variable* r = new Nodes::Variable{right, right_ident->value.buffer, false, {.reg = mapping.C}};
    vars.push(l);
    vars.push(r);
    Nodes::Statement* stmt = parseStmt();
    vars.pop(vars.index(l));
    vars.pop(vars.index(r));
    Nodes::Type* returnType = parseType();
    tryConsumeError(Tokens::TokenType::SEMICOLON, "Expected ';'");

    Nodes::Operation* op = new Nodes::Operation{ident, l, r, stmt, returnType, prec};
    if (declaredOperations.contains(op))
      Errors::error("Operation already declared", peek(-1)->line);
    this->declaredOperations.push(op);
    this->skipStmt = true;
    return new Nodes::Statement{};
  }
  Errors::error("Invalid Statement", peek(-1)->line);
  return {};
}

Lists::List<Nodes::Statement*> Parser::Parser::parseStmts() {
  Lists::List<Nodes::Statement*> ret{};

  while (hasPeek()) {
    Nodes::Statement* stmt = parseStmt();
    if (this->skipStmt) {
      this->skipStmt = false;
    } else {
      ret.push(stmt);
    }
  }
  return ret;
}
