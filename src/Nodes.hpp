#pragma once
#include <iostream>
#include <Literal.hpp>
#include <List.hpp>

namespace Nodes {
  enum class StatementType {
    method
  };
  enum class ExpressionType {
    literal, identifier
  };
  enum class DTypeT {
    MEMBOX, LABEL, STRUCT, UNION
  };

  struct LiteralExpr {
    Literal::Literal lit;
  };
  struct IdentifierExpr {
    char* identifier;
  };

  struct Expression {
    ExpressionType type;
    union {
      LiteralExpr literal;
      IdentifierExpr ident;
    } u;
  };

  struct DataType {
    DTypeT type;
    Expression* expr;
    Lists::List<DataType*> inner;
  };

  struct Statement {
    StatementType type;
    union {

    } u;
    std::string toString();
  };
}

