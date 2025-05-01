#include <Parser/Literal.hpp>
#include "Literal.hpp"

Literals::Literal& Literals::parseLiteral(std::string value) {
  Literal* lit = (Literal*)malloc(sizeof(Literal));
  char last = value.back();

  if (value[0] == '"') {
    lit->type = LiteralType::STRING;
    lit->u.s = (char*)malloc((value.size()-1)*sizeof(char));
    const char* s = value.c_str();
    memcpy(lit->u.s, &(value[1]), (value.size()-2)*sizeof(char));
    lit->u.s[value.size()-1] = 0;
  } else if (value[0] == '\'') {
    lit->type = LiteralType::CHAR;
    lit->u.c = value[1];
  } else if (StringUtils::isInString(last, Constants::LITERAL_PREFIXES)) {
    std::string pure = std::string(value);
    pure.erase(pure.size()-1);

    if (last == Constants::LITERAL_LONG) {
      lit->type = LiteralType::LONG;
      if (StringUtils::isInString('.', pure))
        Errors::error({"Syntax Error", "Cannot cast double literal to a long with prefix"});
      lit->u.l = std::stoll(pure.c_str());
    } else if (last == Constants::LITERAL_FLOAT) {
      lit->type = LiteralType::FLOAT;
      lit->u.f = std::stof(pure.c_str());
    } else if (last == Constants::LITERAL_DOUBLE)  {
      lit->type = LiteralType::DOUBLE;
      lit->u.d = std::stod(pure.c_str());
    } else if (last == Constants::LITERAL_OCTAL) {
      lit->type = LiteralType::INT;
      if (StringUtils::isInString('.', pure))
        Errors::error({"Syntax Error", "Cannot cast double literal to a int with octal prefix"});
      lit->u.i = std::stoi(value.c_str(), NULL, 8);
    } else if (last == Constants::LITERAL_BINARY) {
      lit->type = LiteralType::INT;
      if (StringUtils::isInString('.', pure))
        Errors::error({"Syntax Error", "Cannot cast double literal to a int with binary prefix"});
      lit->u.i = std::stoi(value.c_str(), NULL, 2);
    } else if (last == Constants::LITERAL_HEX) {
      lit->type = LiteralType::INT;
      if (StringUtils::isInString('.', pure))
        Errors::error({"Syntax Error", "Cannot cast double literal to a int with hex prefix"});
      lit->u.i = std::stoi(value.c_str(), NULL, 16);
    }

  } else  {
    if (StringUtils::isInString('.', value)) {
      lit->type = LiteralType::DOUBLE;
      lit->u.d = std::stod(value.c_str());
    } else {
      lit->type = LiteralType::INT;
      lit->u.i = std::stoi(value.c_str());
    }
  }

  return *lit;
}
bool Literals::Literal::operator==(Literal a) {
  if (this->type != a.type)
    return false;
  
  switch (this->type) {
    case LiteralType::CHAR :
      return this->u.c == a.u.c;
    case LiteralType::DOUBLE :
      return this->u.d == a.u.d;
    case LiteralType::FLOAT :
      return this->u.f == a.u.f;
    case LiteralType::INT :
      return this->u.i == a.u.i;
    case LiteralType::LONG :
      return this->u.l == a.u.l;
    case LiteralType::STRING :
      return strcmp(this->u.s, a.u.s) == 0;
  }
  return false;
}