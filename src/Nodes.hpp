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
    literal, identifier, label
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

  struct Method;
  struct LabelExpr {
    Method* method;
  };

  struct Expression {
    ExpressionType type;
    union {
      LiteralExpr literal;
      IdentifierExpr ident;
      LabelExpr label;
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
        case ExpressionType::label :
          return "Label of Method";
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

  struct Type {
    char* name;
    DataType* dt;
    std::string toString() {
      return std::string(name) + "(" + dt->toString() + ")";
    }
    bool operator==(Type a) {
      return std::string(this->name) == std::string(a.name);
    }
  };

  struct Variable {
    Type* type;
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
    Type* returnType;
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
    bool operator==(Method a) {
      if (std::string(this->identifier) != std::string(a.identifier)) return false;
        if (!(*(this->returnType) == *(a.returnType))) return false;
        if (this->params->size() != a.params->size()) return false;
        bool flag = true;
        for (int i = 0; i < this->params->size(); i++) {
          if (!(this->params->at(i)->type == a.params->at(i)->type)) {
            flag = false;
            break;
          }
        }
        return flag;
    }
    std::string toString() {
      std::stringstream ss{};
      ss << ((this->pub) ? "PUBLIC" : "");
      ss << ((this->isInline) ? "INLINE" : "");
      ss << " METHOD(";
      ss << this->returnType->toString();
      ss << "|";
      for (int i = 0; i < this->params->size(); i++) {
        Variable* var = this->params->at(i);
        ss << var->type->toString();
        ss << ",";
      }
      ss << ")";
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

