#pragma once
#include <iostream>
#include <Literal.hpp>
#include <List.hpp>
#include <sstream>
#include <AssemblyTokenizer.hpp>
#include <sstream>

namespace Nodes {
  enum class StatementType {
    method, scope, asm_code, var_decl, var_set
  };
  enum class ExpressionType {
    literal, identifier, label, method_call
  };
  enum class DTypeT {
    MEMBOX, LABEL, STRUCT, UNION, INVALID
  };

  struct Method;
  struct Expression;
  struct LiteralExpr;
  struct Variable;
  struct IdentifierExpr;
  struct LabelExpr;
  struct MethodCall;
  struct Statement;
  struct Scope;
  struct AssemblyCode;
  struct VariableDeclaration;
  struct VariableSetting;

  struct LiteralExpr {
    Literal::Literal lit;
    bool operator==(LiteralExpr a);
  };
  
  struct IdentifierExpr {
    Variable* var;
    bool operator==(IdentifierExpr a);
  };

  struct LabelExpr {
    Method* method;
  };
  struct MethodCall {
    Method* method;
    Lists::List<Expression*>* params;
  };

  struct Expression {
    ExpressionType type;
    union {
      LiteralExpr literal;
      IdentifierExpr ident;
      LabelExpr label;
      MethodCall method_call;
    } u;
    bool operator==(Expression a);
    std::string toString();
  };

  struct DataType {
    DTypeT type;
    Expression* expr;
    Lists::List<Nodes::Variable*> inner;
    std::string toString();
    bool operator==(DataType a);
  };

  struct Type {
    char* name;
    DataType* dt;
    std::string toString();
    bool operator==(Type a);
  };

  struct Variable {
    Type* type;
    char* name;
    bool inStack;
    union {
      char* reg;
      int offset;
    } location;

    std::string toString();
    bool operator==(Variable a);
  };

  struct Scope {
    Lists::List<Statement*> content;
  };

  struct Method {
    char* identifier;
    bool pub;
    bool isInline;
    Type* returnType;
    Lists::List<Variable*>* params;
    Statement* stmt;
    std::string getLabel();
    bool operator==(Method a);
    std::string toString();
  };

  struct AssemblyCode {
    Lists::List<Assembly::Token*>* code;
    std::string toString();
  };

  struct VariableDeclaration {
    Variable* var;
  };
  struct VariableSetting {
    Variable* var;
    Expression* value;
  };

  struct Statement {
    StatementType type;
    union {
      Method* method;
      Scope* scope;
      AssemblyCode* asmCode;
      VariableDeclaration* var_decl;
      VariableSetting* var_set;
    } u;
    std::string toString();
  };
}

