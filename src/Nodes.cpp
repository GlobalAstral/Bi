#include <Nodes.hpp>


std::string Nodes::Statement::toString() {
  std::stringstream ss{};
  switch (this->type) {
    case Nodes::StatementType::method :
      ss << ((this->u.method->pub) ? "PUBLIC" : "");
      ss << ((this->u.method->isInline) ? "INLINE" : "");
      ss << " METHOD(";
      ss << this->u.method->returnType->toString();
      ss << "|";
      for (int i = 0; i < this->u.method->params->size(); i++) {
        Variable* var = this->u.method->params->at(i);
        ss << var->type->toString();
        ss << ",";
      }
      ss << ")";
      break;
    default:
      ss << "NULL";
      break;
  }
  return ss.str();
};
