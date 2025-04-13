#pragma once

#include <iostream>
#include <vector>

namespace Nodes {

  struct Node;

  enum class NodeType {
    scope, method_decl
  };
  
  struct Scope {
    std::vector<Node> nodes;
  };

  struct Method {
    std::string name;
  };

  struct Node {
    NodeType type;
    union {
      Scope* scope;
      Method* method_decl;
    } u;
  };
}
