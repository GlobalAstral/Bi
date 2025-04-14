#include <Parser/Nodes.hpp>
#include "Nodes.hpp"

bool Nodes::Method::operator==(Method other) {
  if (this->name != other.name)
    return false;
  if (*(this->returnType) != *(other.returnType))
    return false;
  if (this->params.size() != other.params.size())
    return false;
  for (int i = 0; i < this->params.size(); i++) {
    if (params[i].type != other.params[i].type)
      return false;
  }
  return true;
}

bool Nodes::Method::operator!=(Method other) {
  return !(*this == other);
}
bool Nodes::Type::operator==(Type other) {
  if (this->type != other.type)
    return false;
  if (this->alias != other.alias)
    return false;
  if (this->mut != other.mut)
    return false;
  if (this->pointsTo != other.pointsTo)
    return false;
  if (this->implementing.size() != other.implementing.size())
    return false;
  if (this->interior.size() != other.interior.size())
    return false;
  if (this->methods.size() != other.methods.size())
    return false;
  
  for (int i = 0; i < implementing.size(); i++) {
    if (implementing[i] != other.implementing[i])
      return false;
  }
  for (int i = 0; i < interior.size(); i++) {
    if (interior[interior.getKey(i)] != other.interior[other.interior.getKey(i)])
      return false;
  }
  for (int i = 0; i < methods.size(); i++) {
    if (methods[i] != other.methods[i])
      return false;
  }
  return true;
}

bool Nodes::Type::operator!=(Type other) {
  return !(*this == other);
}
