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
  struct Variable;
  struct DataType {
    DTypeT type;
    Expression* expr;
    Lists::List<Nodes::Variable*> inner;
    std::string toString();
    bool operator==(DataType a);
  };

  struct Variable {
    DataType* type;
    char* name;
    bool inStack;
    union {
      char* reg;
      int offset;
    } location;

    std::string toString() {
      std::string s = "Variable '" + std::string(this->name) + "' of type '" + this->type->toString() + "' at ";
      if (inStack) {
        char buf[255];
        sprintf(buf, "%d", this->location.offset);
        s = s + "Stack location " + std::string(buf);
      } else {
        s = s + "Register location " + std::string(this->location.reg);
      }
      return s;
    }
    bool operator==(Variable a) {
      if (a.type != this->type) return false;
      if (std::string(a.name) != std::string(this->name)) return false;
      return true;
    }
  };

  struct Statement;

  struct Scope {
    Lists::List<Statement*> content;
  };

  struct Method {
    char* identifier;
    bool pub;
    bool isInline;
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

  struct AssemblyCode {
    Lists::List<Assembly::Token*>* code;
    std::string toString() {
      std::stringstream ss;
      for (int i = 0; i < code->size(); i++) {
        Assembly::Token* tok = code->at(i);
        ss << tok->instruction << " ";
        for (int j = 0; j < tok->params.size(); j++) {
          char* param = tok->params.at(j);
          ss << param;
          if (j < tok->params.size()-1)
            ss << ", ";
        }
        if (i < code->size()-1)
          ss << std::endl;
      }
      return ss.str();
    }
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

