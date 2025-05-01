#include <Parser/Nodes.hpp>

bool Nodes::Method::operator==(const Method other) const {
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

bool Nodes::Method::operator!=(const Method other) const {
  return !(*this == other);
}

bool Nodes::Type::operator==(const Type other) const {
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
  if (*(this->size) != *(other.size))
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

bool Nodes::Type::operator!=(const Type other) const {
  return !(*this == other);
}

bool Nodes::Variable::operator==(const Variable a) const {
  return strcmp(this->name, a.name) == 0;
}

bool Nodes::Variable::operator!=(const Variable a) const {
  return !(*this == a);
}

bool Nodes::Expression::operator==(const Expression a) const {
  if (this->type != a.type)
    return false;
  if (*(this->returnType) != *(a.returnType))
    return false;
  
  switch (this->type) {
    case Nodes::Expression::ExprType::variable :
    case Nodes::Expression::ExprType::reference :
    case Nodes::Expression::ExprType::dereference :
      return this->u.var_expr == a.u.var_expr;
    case Nodes::Expression::ExprType::subscript :
      return this->u.subscript->var == a.u.subscript->var && *(this->u.subscript->expr) == *(a.u.subscript->expr);
    case Nodes::Expression::ExprType::function_call :
      if (this->u.method_call->mtd != a.u.method_call->mtd)
        return false;
      if (this->u.method_call->params.size() != a.u.method_call->params.size())
        return false;
      for (int i = 0; i < this->u.method_call->params.size(); i++) {
        if (this->u.method_call->params[i] != a.u.method_call->params[i])
          return false;
      }
      return true;
    case Nodes::Expression::ExprType::literal :
      return this->u.lit == a.u.lit;
    case Nodes::Expression::ExprType::dot_notation :
      if (this->u.dot_notation->size() != a.u.dot_notation->size())
        return false;
      for (int i = 0; i < this->u.dot_notation->size(); i++) {
        if (this->u.dot_notation[i] != a.u.dot_notation[i])
          return false;
      }
      return true;
    case Nodes::Expression::ExprType::cast :
      return *(this->u.cast_expr->expr) == *(a.u.cast_expr->expr) && this->u.cast_expr->type == a.u.cast_expr->type;
    case Nodes::Expression::ExprType::custom :
      if (this->u.custom->a != a.u.custom->a)
        return false;
      if (this->u.custom->b != a.u.custom->b)
        return false;
      return this->u.custom->op == a.u.custom->op;
  }
  return false;
}

bool Nodes::Expression::operator!=(const Expression a) const {
  return !(*this == a);
}

bool Nodes::Operation::operator==(const Operation a) const {
  if (this->a != a.a)
    return false;
  if (this->b != a.b)
    return false;
  if (this->precedence != a.precedence)
    return false;
  if (this->returnType != a.returnType)
    return false;
  if (strcmp(this->symbols, a.symbols) != 0)
    return false;
  if (this->type != a.type)
    return false;
  return true;
}

bool Nodes::Operation::operator!=(const Operation a) const {
  return !(*this == a);
}
