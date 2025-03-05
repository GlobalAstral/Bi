#pragma once
#include <iostream>
#include <Literal.hpp>
#include <List.hpp>
#include <sstream>
#include <AssemblyTokenizer.hpp>
#include <sstream>
#include <Registers.hpp>

namespace Nodes {
  enum class StatementType {
    method, scope, asm_code, var_decl, var_set, var_init, if_stmt, while_stmt, do_while_stmt, for_stmt, 
    return_stmt, expr_stmt, array_decl, array_set, array_init
  };
  enum class ExpressionType {
    literal, identifier, label, method_call, cast, binary, dereference, reference, array_expr
  };
  enum class DTypeT {
    MEMBOX, LABEL, STRUCT, UNION, INVALID
  };

  struct Type;
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
  struct Operation;
  struct Cast;
  struct Dereference;
  struct Reference;

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

  struct CastExpr {
    Cast* cast;
    Expression* expr;
  };

  struct BinaryExpr {
    Expression* left;
    Expression* right;
    Operation* operation;
  };

  struct Reference {
    Expression* expr;
  };

  struct Dereference {
    Expression* expr;
  };

  struct ArrayExpr {
    Variable* var;
    Expression* index;
  };

  struct Expression {
    ExpressionType type;
    Type* retType;
    union {
      LiteralExpr literal;
      IdentifierExpr ident;
      LabelExpr label;
      MethodCall method_call;
      BinaryExpr bin;
      CastExpr cast;
      Reference ref;
      Dereference deref;
      ArrayExpr array;
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
    Registers::RegisterType regType;
    Type* pointsTo;
    std::string toString();
    bool operator==(Type a);
  };

  struct Operation {
    char* identifier;
    Variable* lType;
    Variable* rType;
    Statement* stmt;
    Type* returnType;
    int precedence;
    bool operator==(Operation a);
    std::string toString();
  };

  struct Cast {
    Variable* input;
    Type* output_type;
    Statement* stmt;
  };

  struct Variable {
    Type* type;
    char* name;
    bool inStack;
    union {
      const char* reg;
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
    bool external;
    std::string getLabel();
    bool operator==(Method a);
    std::string toString();
  };
  struct IfStmt {
    Expression* expr;
    Statement* stmt;
    Statement* else_stmt;
  };

  struct WhileStmt {
    Expression* expr;
    Statement* stmt;
  };

  struct ForStmt {
    Variable* var;
    Expression* expr;
    Statement* action;
    Statement* stmt;
  };

  struct ReturnStmt {
    Expression* expr;
  };

  struct ExprStmt {
    Expression* expr;
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

  struct VariableInitialization {
    Statement* decl;
    Statement* setting;
  };

  struct ArrayDeclaration {
    Variable* var;
    Expression* size;
  };
  struct ArraySetting {
    Variable* var;
    Expression* index;
    Expression* value;
  };

  struct ArrayInitialization {
    Statement* decl;
    Lists::List<Expression*> entries;
  };

  struct Statement {
    StatementType type;
    union {
      Method* method;
      Scope* scope;
      AssemblyCode* asmCode;
      VariableDeclaration* var_decl;
      VariableSetting* var_set;
      VariableInitialization* var_init;
      IfStmt* if_stmt;
      WhileStmt* while_stmt;
      ForStmt* for_stmt;
      ReturnStmt* return_stmt;
      ExprStmt* expr_stmt;
      ArrayDeclaration* array_decl;
      ArraySetting* array_set;
      ArrayInitialization* array_init;
    } u;
    std::string toString();
  };

  Type* getLiteralType(Literal::Literal lit);
}

