#pragma once

#include <iostream>
#include <Utils/Map.hpp>

namespace Types {
  struct Type {
    enum class BuiltinTypes {
      Int, Float, Long, Double, Char, Byte, String, Pointer, Array, Struct, Union, Interface, Custom
    } type;
    Type* pointsTo;
    bool mut;
    std::string alias;
    Map::Map<std::string, Type*> interior;
  };
}

