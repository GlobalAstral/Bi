#pragma once
#include <iostream>
#include <Literal.hpp>

namespace Nodes {
  enum class StatementType {
    method
  };
  enum class ExpressionType {
    literal
  };

  struct LiteralExpr {
    Literal::Literal lit;
  };

  struct Expression {
    ExpressionType type;
    union {
      LiteralExpr lit;
    } u;
  };

  struct Statement {
    StatementType type;
    union {

    } u;
    std::string toString();
  };
}

