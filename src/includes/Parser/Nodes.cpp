#include <Parser/Nodes.hpp>
#include "Nodes.hpp"

bool Nodes::Method::operator==(Method other) {
  if (strcmp(this->name, other.name) != 0)
    return false;
  if (*(this->returnType) != *(other.returnType))
    return false;
  if (this->params.size() != other.params.size())
    return false;
  for (int i = 0; i < this->params.size(); i++) {
    if (*(params.at(i).type) != *(other.params.at(i).type))
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
  if (this->alias != NULL && other.alias != NULL && strcmp(this->alias, other.alias) != 0)
    return false;
  if (this->mut != other.mut)
    return false;
  if (this->pointsTo != NULL && other.pointsTo != NULL && *(this->pointsTo) != *(other.pointsTo))
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
    if (interior[i].type != other.interior[i].type || strcmp(interior[i].name, other.interior[i].name) != 0)
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

bool Nodes::Variable::operator==(Variable a) {
  return strcmp(this->name, a.name) == 0;
}
