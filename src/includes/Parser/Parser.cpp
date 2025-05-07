#include <Parser/Parser.hpp>
#include "Parser.hpp"

Parser::Parser::Parser(std::vector<Tokens::Token> toks) {
  this->content = toks;
}

std::vector<Nodes::Node*> Parser::Parser::parse() {
  std::vector<Nodes::Node*> nodes;

  while (hasPeek()) {
    parseSingle(nodes);
  }

  return nodes;
}

void Parser::Parser::parseSingle(std::vector<Nodes::Node*> &nodes) {
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
    declared_types[ident.value] = tp;
  } else if (tryconsume({Tokens::TokenType::open_curly})) {
    std::vector<Nodes::Node*> scope;
    bool notFound = true;
    int prev_index = variables.size();
    this->inScope = true;
    while (hasPeek()) {
      if (tryconsume({Tokens::TokenType::close_curly})) {
        notFound = false;
        break;
      }
      parseSingle(scope);
    }
    if (notFound)
      error({"Missing Token", "Expected closing curly bracket"});
    for (int i = defers.size()-1; i >= 0; i--) {
      scope.push_back(defers.at(i));
    }
    defers.clear();
    if (variables.size() != prev_index)
      variables.erase(variables.begin()+prev_index, variables.end());
    nodes.push_back(new Nodes::Node{Nodes::NodeType::scope, {.scope = new Nodes::Scope{scope}}});
    this->inScope = false;
  } else if (tryconsume({Tokens::TokenType::ellipsis})) {
    nodes.push_back(new Nodes::Node{Nodes::NodeType::pass});
  } else if (peek().type == Tokens::TokenType::Unary || peek().type == Tokens::TokenType::Binary) {
    Nodes::Operation* op = new Nodes::Operation{};
    if (tryconsume({Tokens::TokenType::Unary}))
      op->type = Nodes::Operation::OpType::unary;
    else if (tryconsume({Tokens::TokenType::Binary}))
      op->type = Nodes::Operation::OpType::binary;

    Tokens::Token symbols = tryconsume({Tokens::TokenType::symbols}, {"Missing Token", "Expected symbols for operator"});
    tryconsume({Tokens::TokenType::colon}, {"Missing Token", "Expected colon"});
    tryconsume({Tokens::TokenType::open_angle}, {"Missing Token", "Expected opening angle bracket"});
    char* syms = (char*)malloc(symbols.value.size()*sizeof(char));
    strcpy(syms, symbols.value.c_str());
    op->symbols = syms;

    Nodes::Type* a_type = parseType();
    Tokens::Token a_ident = tryconsume({Tokens::TokenType::identifier}, {"Missing Token", "Expected Identifier"});
    char* a_name = (char*)malloc(a_ident.value.size()*sizeof(char));
    strcpy(a_name, a_ident.value.c_str());
    Nodes::Variable* var_a = new Nodes::Variable{a_name, a_type};
    Nodes::Variable* var_b = new Nodes::Variable{};
    op->a = var_a;
    variables.push_back(var_a);
    if (op->type == Nodes::Operation::OpType::binary) {
      tryconsume({Tokens::TokenType::comma}, {"Missing Token", "Missing second variable for binary operation"});
      Nodes::Type* b_type = parseType();
      Tokens::Token b_ident = tryconsume({Tokens::TokenType::identifier}, {"Missing Token", "Expected Identifier"});
      char* b_name = (char*)malloc(b_ident.value.size()*sizeof(char));
      strcpy(b_name, b_ident.value.c_str());
      *var_b = {b_name, b_type};
      op->b = var_b;
      variables.push_back(var_b);
    }
    tryconsume({Tokens::TokenType::comma}, {"Missing Token", "Missing comma for operation parameters"});
    op->returnType = parseType();
    tryconsume({Tokens::TokenType::comma}, {"Missing Token", "Missing comma for operation parameters"});
    Nodes::Expression expr = parseExpr();
    if (expr.type != Nodes::Expression::ExprType::literal || expr.u.lit->type != Literals::LiteralType::INT)
      error({"Internal Error", "Operation Precedence must be a literal integer"});
    op->precedence = expr.u.lit->u.i;
    tryconsume({Tokens::TokenType::close_angle}, {"Missing Token", "Expected closing angle bracket"});
    std::vector<Nodes::Node*> temp;
    parseSingle(temp);
    if (temp.size() != 1 || temp.at(0)->type != Nodes::NodeType::scope)
      error({"Syntax Error", "Expected scope as operation body"});
    variables.erase(variables.begin()+VectorUtils::find<Nodes::Variable*>(variables, var_a, [](Nodes::Variable* a, Nodes::Variable* b) {return *a == *b;}));
    if (op->type == Nodes::Operation::OpType::binary)
      variables.erase(variables.begin()+VectorUtils::find<Nodes::Variable*>(variables, var_b, [](Nodes::Variable* a, Nodes::Variable* b) {return *a == *b;}));

    Nodes::Node* o = new Nodes::Node{};
    *o = *(temp.at(0));

    op->stmt = o;
    operations.push_back(op);
  } else if (tryconsume({Tokens::TokenType::method})) {
    Nodes::Method* mtd = new Nodes::Method{};
    *mtd = parseMethodSig(this->methods);
    nodes.push_back(new Nodes::Node{Nodes::NodeType::method_decl, {.method_decl = mtd}});
  } else if (isType()) {
    Nodes::Type* t = parseType();
    Tokens::Token ident = tryconsume({Tokens::TokenType::identifier}, {"Missing Token", "Expected Identifier"});
    ident = applyNamespaces(ident);
    char* name = (char*)malloc(ident.value.size()*sizeof(char));
    strcpy(name, ident.value.c_str());
    Nodes::Variable* var = new Nodes::Variable{name, t};
    if (VectorUtils::find<Nodes::Variable*>(variables, var, [](Nodes::Variable* a, Nodes::Variable* b) {return *a == *b;}) > -1)
      error({"Internal Error", "Variable already exists"});
    variables.push_back(var);
    Nodes::Node* decl = new Nodes::Node{Nodes::NodeType::var_decl, {.var_decl = var}};
    
    if (tryconsume({.type=Tokens::TokenType::symbols, .value="="})) {
      Nodes::Expression& expr = parseExpr();
      if (*t != *(expr.returnType))
        error({"Type Mismatch", Formatting::format("Cannot assign value of type to variable of different type")});
      std::vector<Nodes::Node*>* temp = new std::vector<Nodes::Node*>{};
      temp->push_back(decl);
      temp->push_back(new Nodes::Node{Nodes::NodeType::var_set, {.var_set = new Nodes::VarSet{var, &expr}}});
      nodes.push_back(new Nodes::Node{Nodes::NodeType::var_init, {.pack = temp}});
    } else {
      nodes.push_back(decl);
    }
    tryconsume({Tokens::TokenType::semicolon}, {"Missing Token", "Expected ';'"});
  } else if (tryconsume({Tokens::TokenType::Return})) {
    Nodes::Expression& expr = parseExpr();
    tryconsume({Tokens::TokenType::semicolon}, {"Missing Token", "Expected ';'"});
    nodes.push_back(new Nodes::Node{Nodes::NodeType::returnStmt, {.expr = &expr}});
  } else if (peek().type == Tokens::TokenType::Asm) {
    AssemblyParser asm_parser{variables, consume().value};
    tryconsume({Tokens::TokenType::semicolon}, {"Missing Token", "Expected ';'"});
    std::optional<std::vector<Nodes::AssemblyToken*>> optional = asm_parser.parseText();
    if (!optional.has_value())
      error({"Internal Error", "Variable in assembly code not found"});
    std::vector<Nodes::AssemblyToken*>* toks = new std::vector<Nodes::AssemblyToken*>{};
    *toks = optional.value();
    nodes.push_back(new Nodes::Node{Nodes::NodeType::asm_code, {.asm_code = toks}});
  } else if (tryconsume({Tokens::TokenType::Defer})) {
    if (!this->inScope)
      error({"Internal Error", "Cannot use defer out of scope"});
    std::vector<Nodes::Node*> temp;
    parseSingle(temp);
    if (temp.size() != 1)
      error({"Syntax Error", "Cannot defer multiple statements"});
    defers.push_back(temp.at(0));
  } else if (tryconsume({Tokens::TokenType::If})) {
    tryconsume({Tokens::TokenType::open_paren}, {"Missing Token", "Expected '('"});
    Nodes::Expression& expr = parseExpr();
    tryconsume({Tokens::TokenType::close_paren}, {"Missing Token", "Expected ')'"});
    std::vector<Nodes::Node*> temp;
    parseSingle(temp);
    if (temp.size() != 1)
      error({"Syntax Error", "Expected Statement"});
    Nodes::Node* body = new Nodes::Node{};
    Nodes::Node* second = new Nodes::Node{};
    *body = *(temp.at(0));
    temp.clear();
    if (tryconsume({Tokens::TokenType::Else})) {
      parseSingle(temp);
      if (temp.size() != 1)
        error({"Syntax Error", "Expected Statement"});
      *second = *(temp.at(0));
    }
    nodes.push_back(new Nodes::Node{Nodes::NodeType::if_stmt, {.flow_control=new Nodes::FlowControl{&expr, body, new Nodes::Node{}, second}}});
  } else if (tryconsume({Tokens::TokenType::While})) {
    tryconsume({Tokens::TokenType::open_paren}, {"Missing Token", "Expected '('"});
    Nodes::Expression& expr = parseExpr();
    tryconsume({Tokens::TokenType::close_paren}, {"Missing Token", "Expected ')'"});
    std::vector<Nodes::Node*> temp;
    parseSingle(temp);
    if (temp.size() != 1)
      error({"Syntax Error", "Expected Statement"});
    Nodes::Node* body = new Nodes::Node{};
    *body = *(temp.at(0));
    temp.clear();
    nodes.push_back(new Nodes::Node{Nodes::NodeType::while_stmt, {.flow_control = new Nodes::FlowControl{&expr, body}}});
  } else if (tryconsume({Tokens::TokenType::Do})) {
    std::vector<Nodes::Node*> temp;
    parseSingle(temp);
    if (temp.size() != 1)
      error({"Syntax Error", "Expected Statement"});
    Nodes::Node* body = new Nodes::Node{};
    *body = *(temp.at(0));
    temp.clear();
    tryconsume({Tokens::TokenType::While}, {"Missing Token", "Expected 'while'"});
    tryconsume({Tokens::TokenType::open_paren}, {"Missing Token", "Expected '('"});
    Nodes::Expression& expr = parseExpr();
    tryconsume({Tokens::TokenType::close_paren}, {"Missing Token", "Expected ')'"});
    tryconsume({Tokens::TokenType::semicolon}, {"Missing Token", "Expected ';'"});
    nodes.push_back(new Nodes::Node{Nodes::NodeType::while_stmt, {.flow_control = new Nodes::FlowControl{&expr, body}}});
  } else if (tryconsume({Tokens::TokenType::For})) {
    tryconsume({Tokens::TokenType::open_paren}, {"Missing Token", "Expected '('"});
    std::vector<Nodes::Node*> temp;
    parseSingle(temp);
    if (temp.size() != 1)
      error({"Syntax Error", "Expected Statement"});
    Nodes::Node* init = new Nodes::Node{};
    *init = *(temp.at(0));
    temp.clear();
    if (init->type != Nodes::NodeType::var_init)
      error({"Syntax Error", "Expected variable initialization"});

    Nodes::Expression& expr = parseExpr();
    tryconsume({Tokens::TokenType::semicolon}, {"Missing Token", "Expected ';' separator"});
    parseSingle(temp);
    if (temp.size() != 1)
      error({"Syntax Error", "Expected Statement"});
    Nodes::Node* second = new Nodes::Node{};
    *second = *(temp.at(0));
    temp.clear();
    tryconsume({Tokens::TokenType::close_paren}, {"Missing Token", "Expected ')'"});
    parseSingle(temp);
    if (temp.size() != 1)
      error({"Syntax Error", "Expected Statement"});
    Nodes::Node* body = new Nodes::Node{};
    *body = *(temp.at(0));
    temp.clear();
    variables.erase(variables.begin()+VectorUtils::find<Nodes::Variable*>(variables, init->u.var_set->var, [](Nodes::Variable* a, Nodes::Variable* b){return *a == *b;}));
    nodes.push_back(new Nodes::Node{Nodes::NodeType::for_stmt, {.flow_control=new Nodes::FlowControl{&expr, body, init, second}}});
  } else {
    Nodes::Expression& expr = parseExpr();

    if (tryconsume({.type=Tokens::TokenType::symbols, .value="="})) {
      Nodes::Expression& val = parseExpr();
      switch (expr.type) {
        case Nodes::Expression::ExprType::variable:
        case Nodes::Expression::ExprType::dot_notation:
        case Nodes::Expression::ExprType::dereference:
        case Nodes::Expression::ExprType::subscript:
          nodes.push_back(new Nodes::Node{Nodes::NodeType::assign, {.assign = new Nodes::Assign{&expr, &val}}});
          break;
        default:
          error({"Syntax Error", "Cannot assign to expression."});
      }
    } else {
      if (expr.type != Nodes::Expression::ExprType::function_call)
        error({"Syntax Error", "Expression is non-sense"});
      nodes.push_back(new Nodes::Node{Nodes::NodeType::expression, {.expr = &expr}});
    }
    tryconsume({Tokens::TokenType::semicolon}, {"Missing Token", "Expected Semicolon"});
  }
}

Nodes::Expression& Parser::Parser::parseExpr(bool paren) {
  if (tryconsume({Tokens::TokenType::open_paren}))
    return parseExpr(true);
  Nodes::Expression* expr = (Nodes::Expression*)malloc(sizeof(Nodes::Expression));

  if (peek().type == Tokens::TokenType::literal) {
    expr->type = Nodes::Expression::ExprType::literal;
    Literals::Literal& lit = Literals::parseLiteral(consume().value);
    expr->u.lit = &lit;
    expr->returnType = convertFromLiteral(lit);
  } else if (peek().type == Tokens::TokenType::identifier) {
    Tokens::Token ident = getIdentNamespaces();

    if (tryconsume({Tokens::TokenType::open_paren})) {
      expr->type = Nodes::Expression::ExprType::function_call;
      Nodes::Method* mtd = new Nodes::Method{};
      char* buf = (char*)malloc(ident.value.size()*sizeof(char));
      strcpy(buf, ident.value.c_str());
      mtd->name = buf;
      std::vector<Nodes::Expression> param_values;

      doUntilFind({Tokens::TokenType::close_paren}, [this, &param_values, &mtd](){
        Nodes::Expression e = parseExpr();
        e.returnType->mut = false;
        param_values.push_back(e);
        mtd->params.push_back(new Nodes::Variable{.type = e.returnType});
      }, {Tokens::TokenType::comma}, {"Missing Token", "Missing comma between parameters"});

      std::vector<Nodes::Method*>* found = getMethodsWithArgs(mtd->name, mtd->params);
      Nodes::Method* method;

      if (found->size() == 0)
        error({"Method not Found", Formatting::format("Method '%s' does not exist", ident.value.c_str())});

      if (found->size() > 1) {
        tryconsume({Tokens::TokenType::arrow}, {"Missing Token", "Expected return type specififier"});
        mtd->returnType = parseType();

        int index = VectorUtils::find<Nodes::Method*>(*found, mtd, [](Nodes::Method* a, Nodes::Method* b){return *a == *b;});
        if (index < 0)
          error({"Method not Found", Formatting::format("Method '%s' does not exist", ident.value.c_str())});
        method = found->at(index);
      } else {
        method = found->at(0);
        if (tryconsume({Tokens::TokenType::arrow})) {
          if (*parseType() != *(method->returnType))
            error({"Type Mismatch", "Type in specifier is not the same as only available declared method"});
        }
      }

      expr->returnType = method->returnType;
      expr->u.method_call = new Nodes::MethodCall{method, param_values};
    } else if (peek().type == Tokens::TokenType::dot) {
      expr->type = Nodes::Expression::ExprType::dot_notation;
      char* buf = (char*)malloc(ident.value.size()*sizeof(char));
      strcpy(buf, ident.value.c_str());
      int index = VectorUtils::find<Nodes::Variable*>(this->variables, new Nodes::Variable{buf}, [](Nodes::Variable* a, Nodes::Variable* b){return *a == *b;});
      if (index < 0)
        error({"Variable not Found", Formatting::format("Variable '%s' does not exist", ident.value.c_str())});
      Nodes::Variable* var = (Nodes::Variable*)malloc(sizeof(Nodes::Variable));
      *var = *(variables.at(index));
      
      expr->u.dot_notation = new std::vector<Nodes::Variable>{};
      expr->u.dot_notation->push_back(*var);
      while (tryconsume({Tokens::TokenType::dot})) {
        Tokens::Token id = tryconsume({Tokens::TokenType::identifier}, {"Missing Token", "Expected Identifier"});
        char* buffer = (char*)malloc(id.value.size()*sizeof(char));
        strcpy(buffer, id.value.c_str());
        if (var->type->type != Nodes::Type::Builtins::Struct && var->type->type != Nodes::Type::Builtins::Union)
          error({"Internal Error", "Cannot use dot notation on non struct or union type"});
        int i = VectorUtils::find<Nodes::Variable*>(var->type->interior, new Nodes::Variable{buffer}, [](Nodes::Variable* a, Nodes::Variable* b){return *a == *b;});
        if (i < 0)
          error({"Variable not Found", Formatting::format("Variable '%s' does not exist", id.value.c_str())});
        Nodes::Variable* v = (Nodes::Variable*)malloc(sizeof(Nodes::Variable));
        *v = *(var->type->interior.at(i));
        expr->u.dot_notation->push_back(*v);
        *var = *v;
      }
      expr->returnType = var->type;
    } else {
      expr->type = Nodes::Expression::ExprType::variable;
      char* buf = (char*)malloc(ident.value.size()*sizeof(char));
      strcpy(buf, ident.value.c_str());
      int index = VectorUtils::find<Nodes::Variable*>(this->variables, new Nodes::Variable{buf}, [](Nodes::Variable* a, Nodes::Variable* b){return *a == *b;});
      if (index < 0)
        error({"Variable not Found", Formatting::format("Variable '%s' does not exist", ident.value.c_str())});
      Nodes::Variable* var = (Nodes::Variable*)malloc(sizeof(Nodes::Variable));
      *var = *(variables.at(index));
      expr->returnType = var->type;
      expr->u.var_expr = var;
    }
  } else if (tryconsume({.type=Tokens::TokenType::symbols, .value="*"})) {
    Nodes::Expression& e = parseExpr();
    
    if (e.returnType->type != Nodes::Type::Builtins::Pointer)
      error({"Internal Error", "Cannot dereference a non pointer type"});
    
    expr->type = Nodes::Expression::ExprType::dereference;
    expr->returnType = e.returnType->pointsTo;
    expr->u.expr = &e;
  } else if (tryconsume({.type=Tokens::TokenType::symbols, .value="&"})) {
    Nodes::Expression& e = parseExpr();
    
    expr->type = Nodes::Expression::ExprType::reference;
    expr->returnType = new Nodes::Type{Nodes::Type::Builtins::Pointer, e.returnType, false,};
    expr->u.expr = &e;
  } else if (peek().type == Tokens::TokenType::symbols) {
    Tokens::Token syms = consume();
    Nodes::Expression& ex = parseExpr();
    char* symbols = (char*)malloc(syms.value.size()*sizeof(char));
    strcpy(symbols, syms.value.c_str());

    int index = VectorUtils::find<Nodes::Operation*>(this->operations, new Nodes::Operation{.symbols = symbols}, [](Nodes::Operation* a, Nodes::Operation* b) {
      return strcmp(a->symbols, b->symbols) == 0;
    });
    if (index < 0)
      error({"Internal Error", "Operation never declared"});
    
    Nodes::Operation* op = operations.at(index);
    if (op->type != Nodes::Operation::OpType::unary)
      error({"Internal Error", "Cannot use non-unary operation with single expression"});
    
    Nodes::CustomExpr* c_expr = new Nodes::CustomExpr{};
    c_expr->op = op;
    c_expr->a = &ex;

    expr = new Nodes::Expression{Nodes::Expression::ExprType::custom, c_expr->op->returnType, {.custom = c_expr}};
  } else {
    error({"Syntax Error", "Invalid Expression"});
  }

  if (tryconsume({Tokens::TokenType::As})) {
    Nodes::Expression* ex = new Nodes::Expression{};
    *ex = *expr;
    Nodes::CastExpr* cast = new Nodes::CastExpr{ex, parseType()};
    expr->type = Nodes::Expression::ExprType::cast;
    expr->returnType = cast->type;
    expr->u.cast_expr = cast;
  }

  if (tryconsume({Tokens::TokenType::open_square})) {
    if (expr->returnType->type != Nodes::Type::Builtins::Array && expr->returnType->type != Nodes::Type::Builtins::Pointer)
      error({"Internal Error", "Cannot index a non array or non pointer type"});
    Nodes::Expression* ex = new Nodes::Expression{};
    ex->type = Nodes::Expression::ExprType::subscript;
    Nodes::Expression& e = parseExpr();
    ex->returnType = expr->returnType->pointsTo;
    ex->u.subscript = new Nodes::SubscriptExpr{expr, &e};
    tryconsume({Tokens::TokenType::close_square}, {"Missing Token", "Expected closing square bracket"});
  }

  if (peek().type == Tokens::TokenType::symbols && peek().value != "=") {
    Tokens::Token syms = consume();
    char* symbols = (char*)malloc(syms.value.size()*sizeof(char));
    strcpy(symbols, syms.value.c_str());

    int index = VectorUtils::find<Nodes::Operation*>(this->operations, new Nodes::Operation{.symbols = symbols}, [](Nodes::Operation* a, Nodes::Operation* b) {
      return strcmp(a->symbols, b->symbols) == 0;
    });
    if (index < 0)
      error({"Internal Error", "Operation never declared"});
    
    Nodes::Operation* op = operations.at(index);
    Nodes::CustomExpr* c_expr = new Nodes::CustomExpr{};
    c_expr->op = op;
    Nodes::Expression& right = parseExpr();
    if (right.type == Nodes::Expression::ExprType::custom && right.u.custom->op->precedence < c_expr->op->precedence) {
      c_expr->a = expr;
      c_expr->b = right.u.custom->a;
      right.u.custom->a = new Nodes::Expression{Nodes::Expression::ExprType::custom, c_expr->op->returnType, {.custom = c_expr}};
      expr = &right;
    } else {
      c_expr->a = expr;
      c_expr->b = &right;
      expr = new Nodes::Expression{Nodes::Expression::ExprType::custom, c_expr->op->returnType, {.custom = c_expr}};
    }

    if ((expr->u.custom->op->a != NULL && expr->u.custom->a != NULL && *(expr->u.custom->a->returnType) != *(expr->u.custom->op->a->type) ) || ((expr->u.custom->op->b != NULL && expr->u.custom->b != NULL) && *(expr->u.custom->b->returnType) != *(expr->u.custom->op->b->type)))
      error({"Type Mismatch", "No operation exists with provided operands"});
  }

  if (paren)
    tryconsume({Tokens::TokenType::close_paren}, {"Missing Token", "Expected closing parenthesis"});

  return *expr;
  
}

Tokens::Token& Parser::Parser::getIdentNamespaces() {
  using std::stringstream;
  Tokens::Token ident = tryconsume({Tokens::TokenType::identifier}, {"Missing Token", "Expected Identifier"});
  Tokens::Token* ret = new Tokens::Token{};
  *ret = ident;
  stringstream buf;
  buf << ret->value;
  while (tryconsume({Tokens::TokenType::colon})) {
    *ret = tryconsume({Tokens::TokenType::identifier}, {"Missing Token", "Expected Identifier after colon for namespace"});
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
  if (tryconsume({.type=Tokens::TokenType::symbols, .value="*"})) {
    Nodes::Type* temp = parseType();
    *t = {Nodes::Type::Builtins::Pointer, temp};
  } else if (tryconsume({Tokens::TokenType::open_square})) {
    Nodes::Expression& ex = parseExpr();
    tryconsume({Tokens::TokenType::close_square}, {"Missing Token", "Expected closing square bracket"});
    Nodes::Type* temp = parseType();
    *t = {Nodes::Type::Builtins::Array, temp};
    t->size = &ex;
  } else if (tryconsume({Tokens::TokenType::Int})) {
    *t = {Nodes::Type::Builtins::Int};
  } else if (tryconsume({Tokens::TokenType::Uint})) {
    *t = {Nodes::Type::Builtins::Uint};
  } else if (tryconsume({Tokens::TokenType::Float})) {
    *t = {Nodes::Type::Builtins::Float};
  } else if (tryconsume({Tokens::TokenType::Long})) {
    *t = {Nodes::Type::Builtins::Long};
  } else if (tryconsume({Tokens::TokenType::Ulong})) {
    *t = {Nodes::Type::Builtins::Ulong};
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
    if (tryconsume({Tokens::TokenType::colon})) {
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
        std::vector<Nodes::Method*> temp;
        Nodes::Method mtd = parseMethodSig(temp);
        if (mtd.scope == NULL)
          error({"Syntax Error", "Cannot declare method inside struct. Use an interface and implement it instead"});
        for (Nodes::Type* type : t->implementing) {
          bool found = false;
          for (int i = 0; i < type->methods.size(); i++) {
            if (*(type->methods[i]) == mtd) {
              found = true;
              type->methods[i]->scope = mtd.scope;
            }
          }
          if (!found)
            error({"Internal Error", "Cannot find method in implementations for struct"});
        }
      } else {
        Nodes::Type* tp = parseType();
        Tokens::Token ident = tryconsume({Tokens::TokenType::identifier}, {"Missing Token", "Expected Identifier"});
        tryconsume({Tokens::TokenType::semicolon}, {"Missing Token", "Expected semicolon"});
        char* buf = (char*)malloc(ident.value.size()*sizeof(char));
        strcpy(buf, ident.value.c_str());
        Nodes::Variable* var = new Nodes::Variable{buf, tp};
        int index = VectorUtils::find<Nodes::Variable*>(t->interior, var, [](Nodes::Variable* a, Nodes::Variable* b){return *a == *b;});
        if (index > -1)
          error({"Internal Error", Formatting::format("Struct member '%s' already exists", buf)});
        t->interior.push_back(var);
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
      char* buf = (char*)malloc(ident.value.size()*sizeof(char));
      strcpy(buf, ident.value.c_str());
      Nodes::Variable* var = new Nodes::Variable{buf, tp};
      int index = VectorUtils::find<Nodes::Variable*>(t->interior, var, [](Nodes::Variable* a, Nodes::Variable* b){return *a == *b;});
      if (index > -1)
        error({"Internal Error", Formatting::format("Union member '%s' already exists", buf)});
      t->interior.push_back(var);
    });
    if (!found)
      error({"Missing Token", "Expected closing curly bracket"});
  } else if (tryconsume({Tokens::TokenType::Interface})) {
    t->type = Nodes::Type::Builtins::Interface;
    
    tryconsume({Tokens::TokenType::open_curly}, {"Missing Token", "Expected opening curly bracket after interface"});
    bool found = doUntilFind({Tokens::TokenType::close_curly}, [this, &t](){
      Nodes::Method mtd = parseMethodSig(t->methods);
    });
    if (!found)
      error({"Missing Token", "Expected closing curly bracket"});
  } else if (peek().type == Tokens::TokenType::identifier) {
    if (!declared_types.contains(peek().value))
      error({"Invalid Type", Formatting::format("Identifier '%s' does not name a type", consume().value.c_str())});
    *t = {Nodes::Type::Builtins::Custom};
    std::string val = consume().value;
    char* buf = (char*)malloc(val.size()*sizeof(char));
    strcpy(buf, val.c_str());
    t->alias = buf;
  }

  t->mut = tryconsume({Tokens::TokenType::Mutable});
  return t;
}

Nodes::Type *Parser::Parser::convertFromLiteral(Literals::Literal lit) {
  Nodes::Type* ret = (Nodes::Type*)malloc(sizeof(Nodes::Type));
  ret->mut = false;

  switch (lit.type) {
    case Literals::LiteralType::INT :
      ret->type = Nodes::Type::Builtins::Uint;
      break;
    case Literals::LiteralType::LONG :
      ret->type = Nodes::Type::Builtins::Ulong;
      break;
    case Literals::LiteralType::FLOAT :
      ret->type = Nodes::Type::Builtins::Float;
      break;
    case Literals::LiteralType::DOUBLE :
      ret->type = Nodes::Type::Builtins::Double;
      break;
    case Literals::LiteralType::CHAR :
      ret->type = Nodes::Type::Builtins::Char;
      break;
    case Literals::LiteralType::STRING :
      ret->type = Nodes::Type::Builtins::String;
      break;
  }
  return ret;
}

Nodes::Method Parser::Parser::parseMethodSig(std::vector<Nodes::Method*>& mtds) {
  bool Inline = tryconsume({Tokens::TokenType::Inline});
  Nodes::Type* retType = parseType();
  Tokens::Token ident = tryconsume({Tokens::TokenType::identifier}, {"Missing Token", "Expected Identifier"});
  ident = applyNamespaces(ident);
  std::vector<Nodes::Variable*> params;
  Nodes::Node* body = new Nodes::Node{};

  char* buf = (char*)malloc(ident.value.size()*sizeof(char));
  strcpy(buf, ident.value.c_str());

  if (tryconsume({Tokens::TokenType::open_paren})) {
    bool found = doUntilFind({Tokens::TokenType::close_paren}, [this, &params](){
      Nodes::Type* paramType = parseType();
      Tokens::Token paramIdent = tryconsume({Tokens::TokenType::identifier}, {"Missing Token", "Expected Identifier"});
      if (peek().type != Tokens::TokenType::comma && peek().type != Tokens::TokenType::close_paren)
        error({"Unexpected Token", Formatting::format("Token '%s' is not comma or closing parenthesis", consume().toString().c_str())});
      tryconsume({Tokens::TokenType::comma});
      char* buf = (char*)malloc(paramIdent.value.size()*sizeof(char));
      strcpy(buf, paramIdent.value.c_str());
      params.push_back(new Nodes::Variable{buf, paramType});
    });
    if (!found)
      error({"Missing Token", "Expected closing parenthesis"});
  }
  Nodes::Method* mtd = new Nodes::Method{retType, buf, params, body, Inline};
  if (VectorUtils::find<Nodes::Method*>(mtds, mtd, [](Nodes::Method* a, Nodes::Method* b){return *a == *b;}) > -1)
    error({"Internal Error", "Method already exists"});
  mtds.push_back(mtd);
  if (!tryconsume({Tokens::TokenType::semicolon})) {
    int prev_index = variables.size()-1;
    for (auto param : params)
      variables.push_back(param);
    std::vector<Nodes::Node*> temp;
    parseSingle(temp);
    variables.erase(variables.begin()+prev_index, variables.end());

    if (temp.size() != 1 || temp.at(0)->type != Nodes::NodeType::scope)
      error({"Syntax Error", "Expected scope as method body"});
    
    mtds.back()->scope = temp.at(0);
  }

  return *mtd;
}

Tokens::Token Parser::Parser::null() {
  return Tokens::nullToken();
}

bool Parser::Parser::isType() {
  return peek().type == Tokens::TokenType::Int || peek().type == Tokens::TokenType::Long || peek().type == Tokens::TokenType::Float || peek().type == Tokens::TokenType::Double || peek().type == Tokens::TokenType::Byte || peek().type == Tokens::TokenType::Char || peek().type == Tokens::TokenType::String || peek().type == Tokens::TokenType::Struct || peek().type == Tokens::TokenType::Union || peek().type == Tokens::TokenType::Interface || peek().type == Tokens::TokenType::Void || peek().type == Tokens::TokenType::Uint || peek().type == Tokens::TokenType::Ulong || (peek().type == Tokens::TokenType::identifier && declared_types.contains(peek().value)); 
}

std::vector<Nodes::Method*>* Parser::Parser::getMethodsWithArgs(char *name, std::vector<Nodes::Variable*> params) {
  std::vector<Nodes::Method*>* ret = new std::vector<Nodes::Method*>{};
  for (auto mtd : methods) {
    if (strcmp(mtd->name, name) == 0 && mtd->params.size() == params.size()) {
      bool equal = true;
      for (int i = 0; i < mtd->params.size(); i++) {
        if (*(mtd->params.at(i)->type) != *(params.at(i)->type))
          equal = false;
      }
      if (equal)
        ret->push_back(mtd);
    }
  }
  return ret;
}

int Parser::Parser::getCurrentLine() {
  return peek(-1).line;
}

bool Parser::Parser::equalCriteria(Tokens::Token a, Tokens::Token b) {
  return a.type == b.type && ((!a.value.empty() && !b.value.empty()) ? a.value == b.value : true);
}

Parser::AssemblyParser::AssemblyParser(std::vector<Nodes::Variable*> &vars, std::string txt) {
  variables = &vars;
  text = txt;
}

std::optional<std::vector<Nodes::AssemblyToken*>> Parser::AssemblyParser::parseText() {
  std::vector<Nodes::AssemblyToken*> ret;
  char* buf = (char*)malloc(text.size()*sizeof(char));
  strcpy(buf, text.c_str());

  std::stringstream stream;

  while (*buf) {
    if (*buf == '%') {
      if (!stream.str().empty()) {
        char* txt = (char*)malloc(stream.str().size()*sizeof(char));
        strcpy(txt, stream.str().c_str());
        ret.push_back(new Nodes::AssemblyToken{Nodes::AssemblyToken::AsmTokenType::text, {.text = txt}});
        stream.str("");
      }

      buf++;
      std::stringstream ss;
      while (*buf != '%') {
        ss << *buf;
        buf++;
      }
      buf++;

      char* name = (char*)malloc(ss.str().size()*sizeof(char));
      strcpy(name, ss.str().c_str());

      int index = VectorUtils::find<Nodes::Variable*>(*variables, new Nodes::Variable{name}, [](Nodes::Variable* a, Nodes::Variable* b){return *a == *b;});
      if (index < 0)
        return {};
      ret.push_back(new Nodes::AssemblyToken{Nodes::AssemblyToken::AsmTokenType::var, {.var = variables->at(index)}});
    } else {
      stream << *buf;
      buf++;
    }
  }
  return ret;
}
