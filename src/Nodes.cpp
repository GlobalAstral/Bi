#include <Nodes.hpp>

std::string Nodes::DataType::toString() {
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

bool Nodes::DataType::operator==(DataType a) {
  if (this->type != a.type) return false;
  if (!(*expr == *(a.expr))) return false;
  if (inner.size() != a.inner.size()) return false;
  bool flag = true;
  for (int i = 0; i < inner.size(); i++) {
    Variable* j = inner.at(i);
    Variable* k = a.inner.at(i);
    if (!(*j == *k)) {
      flag = false;
      break;
    }
  }
  return flag;
}

std::string Nodes::Statement::toString() {
  std::stringstream ss{};
  switch (this->type) {
    case Nodes::StatementType::method :
      ss << this->u.method->toString();
      break;
    case Nodes::StatementType::asm_code :
      ss << "Assembly code '" << this->u.asmCode->toString() << "'";
      break;
    case Nodes::StatementType::var_decl :
      ss << "Declaration of variable '" << this->u.var_decl->var->toString() << "'";
      break;
    case Nodes::StatementType::var_set :
      ss << "Setting of variable '" << this->u.var_set->var->toString() << "' to value '" << this->u.var_set->value->toString() << "'";
      break;
    default:
      ss << "NULL";
      break;
  }
  return ss.str();
};
