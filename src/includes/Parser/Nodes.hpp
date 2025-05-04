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
      Int, Uint, Float, Ulong, Long, Double, Char, Byte, String, Pointer, Array, Struct, Union, Interface, Void, Custom
    } type;
    Type* pointsTo;
    bool mut;
    char* alias;
    std::vector<Variable> interior;
    std::vector<Nodes::Method> methods;
    std::vector<Type*> implementing;
    Expression* size;

    bool operator==(const Type other) const;
    bool operator!=(const Type other) const;
  };

  enum class NodeType {
    pass, scope, method_decl, var_decl, var_set, returnStmt, asm_code, defer,
  };

  struct Scope {
    std::vector<Node> nodes;
  };

  struct Variable {
    char* name;
    Nodes::Type* type;
    bool operator==(const Variable a) const;
    bool operator!=(const Variable a) const;
  };

  struct Method {
    Nodes::Type* returnType;
    char* name;
    std::vector<Variable> params;
    Node* scope;
    bool Inline;
    bool operator==(const Method other) const;
    bool operator!=(const Method other) const;
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
    Node* stmt;

    bool operator==(const Operation a) const;
    bool operator!=(const Operation a) const;
  };

  struct VarSet {
    Variable var;
    Expression* expr;
  };

  struct AssemblyToken {
    enum class AsmTokenType {
      var, text
    } type;
    union {
      Variable var;
      char* text;
    } u;
  };

  struct Node {
    NodeType type;
    union {
      Scope* scope;
      Method* method_decl;
      Variable* var_decl;
      VarSet* var_set;
      Expression* expr;
      std::vector<Nodes::AssemblyToken>* asm_code;
    } u;

    std::string toString() const;
  };

  struct MethodCall {
    Method* mtd;
    std::vector<Expression> params;
  };

  struct SubscriptExpr {
    Expression* expr;
    Expression* index;
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
      Expression* expr;
      MethodCall* method_call;
      SubscriptExpr* subscript;
      std::vector<Variable>* dot_notation;
      CastExpr* cast_expr;
      CustomExpr* custom;
    } u;

    bool operator==(const Expression a) const;
    bool operator !=(const Expression a) const;
  };
}
