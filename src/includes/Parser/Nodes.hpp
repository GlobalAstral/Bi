#pragma once

#include <iostream>
#include <vector>
#include <Utils/Map.hpp>
#include <Parser/Literal.hpp>
#include <string.h>

namespace Nodes {

  struct Node;
  struct Method;
  struct Expression;
  struct Variable;

  struct Type {
    enum class Builtins {
      Int, Float, Long, Double, Char, Byte, String, Pointer, Array, Struct, Union, Interface, Void, Custom
    } type;
    Type* pointsTo;
    bool mut;
    char* alias;
    std::vector<Variable> interior;
    std::vector<Nodes::Method> methods;
    std::vector<Type*> implementing;
    bool Unsigned;

    bool operator==(Type other);
    bool operator!=(Type other);
  };

  enum class NodeType {
    pass, scope, method_decl, 
  };

  struct Scope {
    std::vector<Node> nodes;
  };

  struct Variable {
    char* name;
    Nodes::Type* type;
    bool operator==(Variable a);
  };

  struct Method {
    Nodes::Type* returnType;
    char* name;
    std::vector<Variable> params;
    std::vector<Node> content;
    bool operator==(Method other);
    bool operator!=(Method other);
  };

  struct Operation {
    enum class OpType {
      unary,
      binary
    } type;
    char* symbols;
    Variable a;
    Variable b;
    Type* returnType;
    int precedence;
    std::vector<Node>* stmt;
  };

  struct Node {
    NodeType type;
    union {
      Scope* scope;
      Method* method_decl;
    } u;
  };

  struct MethodCall {
    Method* mtd;
    std::vector<Expression> params;
  };

  struct SubscriptExpr {
    Variable var;
    Expression* expr;
  };

  struct CastExpr {
    Expression* expr;
    Type* type;
  };

  struct CustomExpr {
    Expression* a;
    Expression* b;
    Operation op;
  };

  struct Expression {
    enum class ExprType {
      custom,
      literal,
      variable,
      function_call,
      dereference,
      reference,
      subscript,
      dot_notation,
      cast
    } type;

    Type* returnType;
    union {
      Literals::Literal* lit;
      Variable* var_expr;
      MethodCall* method_call;
      SubscriptExpr* subscript;
      std::vector<Variable>* dot_notation;
      CastExpr* cast_expr;
      CustomExpr* custom;
    } u;
  };
}
