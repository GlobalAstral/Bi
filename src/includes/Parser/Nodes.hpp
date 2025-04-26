#pragma once

#include <iostream>
#include <vector>
#include <Utils/Map.hpp>

namespace Nodes {

  struct Node;
  struct Method;

  struct Type {
    enum class Builtins {
      Int, Float, Long, Double, Char, Byte, String, Pointer, Array, Struct, Union, Interface, Void, Custom
    } type;
    Type* pointsTo;
    bool mut;
    std::string alias;
    Map::Map<std::string, Type*> interior;
    std::vector<Nodes::Method> methods;
    std::vector<Type*> implementing;

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
    std::string name;
    Nodes::Type* type;
  };

  struct Method {
    Nodes::Type* returnType;
    std::string name;
    std::vector<Variable> params;
    std::vector<Node> content;
    bool operator==(Method other);
    bool operator!=(Method other);
  };

  struct Node {
    NodeType type;
    union {
      Scope* scope;
      Method* method_decl;
    } u;
  };

  struct Expression {
    enum class ExprType {
      custom,
      literal,
      variable,
      function_call,
      reference,
      dereference,
      subscript,
      dot_notation
    } type;

    union {

    } u;

    //TODO All types of expression
  };
  
}
