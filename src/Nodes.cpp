#include <Nodes.hpp>

#define LITERAL_TYPE "0literal"

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
    case Nodes::StatementType::scope :
      ss << "Scope";
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
    case Nodes::StatementType::var_init :
      ss << "Init of variable '" << this->u.var_init->decl->toString() << "' to '" << this->u.var_init->setting->toString() << "'";
      break;
    case Nodes::StatementType::if_stmt :
      ss << "if (";
      ss << this->u.if_stmt->expr->toString();
      ss << ") then ";
      ss << this->u.if_stmt->stmt->toString();
      ss << " else ";
      ss << this->u.if_stmt->else_stmt->toString();
      break;
    default:
      ss << "NULL";
      break;
  }
  return ss.str();
};

bool Nodes::IdentifierExpr::operator==(IdentifierExpr a)  {
  return std::string(this->var->name) == std::string(a.var->name);
}

std::string Nodes::Expression::toString()  {
  std::stringstream ss;
  switch (this->type) {
    case ExpressionType::identifier :
      return std::string(this->u.ident.var->name);
    case ExpressionType::literal :
      return this->u.literal.lit.toString();
    case ExpressionType::label :
      return "Label of Method";
    case ExpressionType::method_call :
      return "Call of Method";
    case ExpressionType::binary :
      return "BINARY(" + this->u.bin.left->toString() + " | " + this->u.bin.right->toString() + ", " + this->u.bin.operation->toString() + ")";
    case ExpressionType::cast :
      return this->u.cast.expr->toString() + " AS " + this->u.cast.cast->output_type->toString();
    default:
      return "NULL";
  }
}

bool Nodes::LiteralExpr::operator==(LiteralExpr a) {
  return this->lit == a.lit;
}

bool Nodes::Expression::operator==(Expression a)  {
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

std::string Nodes::Type::toString() {
  return (this == NULL) ? "VOID" : std::string(name) + "(" + dt->toString() + ") which points to (" + pointsTo->toString() + ")";
}

bool Nodes::Type::operator==(Type a) {
  return std::string(this->name) == std::string(a.name);
}

std::string Nodes::Variable::toString() {
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

bool Nodes::Variable::operator==(Variable a) {
  if (a.type != this->type) return false;
  if (std::string(a.name) != std::string(this->name)) return false;
  return true;
}

std::string Nodes::Method::getLabel() {
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

bool Nodes::Method::operator==(Method a) {
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

std::string Nodes::Method::toString() {
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

std::string Nodes::AssemblyCode::toString() {
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

Nodes::Type* Nodes::getLiteralType(Literal::Literal literal) {
  int size;
  switch (literal.type) {
    case Literal::LiteralType::integer:
    case Literal::LiteralType::floating:
      size = 4;
      break;
    case Literal::LiteralType::long_int:
    case Literal::LiteralType::binary:
    case Literal::LiteralType::hexadecimal:
    case Literal::LiteralType::string:
    case Literal::LiteralType::double_floating:
      size = 8;
      break;
    case Literal::LiteralType::character:
      size = 1;
      break;
    default:
      Errors::error("Invalid Literal");
  }
  Literal::Literal lit = {Literal::LiteralType::integer, {.i = size}};
  Nodes::Expression* e = new Nodes::Expression{Nodes::ExpressionType::literal, new Nodes::Type{}, {.literal = {lit}}};
  Nodes::DataType* dt = new Nodes::DataType{Nodes::DTypeT::MEMBOX, e};
  Registers::RegisterType reg;
  if (size*8 == 8)
    reg = Registers::RegisterType::b8;
  else if (size*8 == 16)
    reg = Registers::RegisterType::b16;
  else if (size*8 == 32)
    if (literal.type == Literal::LiteralType::floating)
      reg = Registers::RegisterType::simd;
    else
      reg = Registers::RegisterType::b32;
  else if (size*8 == 64)
    if (literal.type == Literal::LiteralType::floating)
      reg = Registers::RegisterType::simd;
    else
      reg = Registers::RegisterType::b64;
  return new Nodes::Type{const_cast<char*>(LITERAL_TYPE), dt, reg};
}

bool Nodes::Operation::operator==(Operation a) {
  if (std::string(this->identifier) != std::string(a.identifier)) return false;
  if (!(this->returnType == a.returnType)) return false;
  if (!(this->lType == a.lType)) return false;
  if (!(this->rType == a.rType)) return false;
  return true;
}

std::string Nodes::Operation::toString() {
  return "Operation(" + this->lType->toString() + ", " + this->rType->toString() + ", " + this->identifier + " | " + this->returnType->toString() + ")";
}
