#pragma once
#include <iostream>
#include <Literal.hpp>
#include <List.hpp>
#include <sstream>

namespace Nodes {
  enum class StatementType {
    method, scope
  };
  enum class ExpressionType {
    literal, identifier
  };
  enum class DTypeT {
    MEMBOX, LABEL, STRUCT, UNION, INVALID
  };

  struct LiteralExpr {
    Literal::Literal lit;
    bool operator==(LiteralExpr a) {
      return this->lit == a.lit;
    }
  };
  struct IdentifierExpr {
    char* identifier;
    bool operator==(IdentifierExpr a) {
      return std::string(this->identifier) == std::string(a.identifier);
    }
  };

  struct Expression {
    ExpressionType type;
    union {
      LiteralExpr literal;
      IdentifierExpr ident;
    } u;
    bool operator==(Expression a) {
      if (this->type != a.type) return false;
      switch (this->type) {
        case ExpressionType::identifier :
          return this->u.ident == a.u.ident;
        case ExpressionType::literal :
          return this->u.literal == a.u.literal;
        default:
          return false;
      }
      return false;
    }
    std::string toString() {
      switch (this->type) {
        case ExpressionType::identifier :
          return std::string(this->u.ident.identifier);
        case ExpressionType::literal :
          return this->u.literal.lit.toString();
        default:
          return "NULL";
      }
    }
  };

  struct DataType {
    DTypeT type;
    Expression* expr;
    Lists::List<DataType*> inner;
    std::string toString() {
      std::stringstream ss;
      switch (this->type) {
        case DTypeT::MEMBOX :
          return "MEMBOX(" + this->expr->toString() + ")";
        case DTypeT::LABEL :
          return "LABEL";
        case DTypeT::STRUCT :
          for (int i = 0; i < this->inner.size(); i++)
            ss << this->inner.at(i)->toString() << ", ";
          return "STRUCT(" + ss.str() + ")";
        case DTypeT::UNION :
          for (int i = 0; i < this->inner.size(); i++)
            ss << this->inner.at(i)->toString() << ", ";
          return "UNION(" + ss.str() + ")";
        default:
          return "NULL";
      }
    }
    bool operator==(DataType a) {
      if (this->type != a.type) return false;
      if (!(*expr == *(a.expr))) return false;
      if (inner.size() != a.inner.size()) return false;
      bool flag = true;
      for (int i = 0; i < inner.size(); i++) {
        DataType* j = inner.at(i);
        DataType* k = a.inner.at(i);
        if (!(*j == *k)) {
          flag = false;
          break;
        }
      }
      return flag;
    }
  };

  struct Variable {
    DataType* type;
    char* name;
    bool inStack;
    union {
      char* reg;
      int offset;
    } location;
  };

  struct Statement;

  struct Scope {
    Lists::List<Statement*> content;
  };

  struct Method {
    char* identifier;
    bool pub;
    DataType* returnType;
    Lists::List<Variable*>* params;
    Statement* stmt;
    std::string getLabel() {
      std::stringstream ss;
      ss << identifier;
      ss << "_";
      ss << returnType->toString();
      ss << "_";
      for (int i = 0; i < params->size(); i++) {
        ss << params->at(i)->type->toString();
        ss << "_";
      }
      return ss.str();
    }
  };

  struct Statement {
    StatementType type;
    union {
      Method* method;
      Scope* scope;
    } u;
    std::string toString();
  };
}

