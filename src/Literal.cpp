#include <Literal.hpp>

bool Literal::isSuffix(char c) {
  return c == 'f' || c == 'L' || c == 'b' || c == 'd';
}

bool Literal::hasDot(std::string s) {
  for (char c : s) 
    if (c == '.')
      return true;
  return false;
}

Literal::Literal Literal::parseLiteral(std::string s, int line) {
  char last = s.at(s.size()-1);
  std::string f2 = s.substr(0, 2);
  bool floating = hasDot(s);
  if (f2 == HEX_PREFIX) {
    if (floating) Errors::error("Cannot have floating hex value", line);
    return {LiteralType::hexadecimal, {.h = std::stoull(s, 0, 16)}};
  }
  if (floating) {
    if (!isSuffix(last)) 
      return {LiteralType::double_floating, {.d = std::stod(s)}};
    if (last == 'f') 
      return {LiteralType::floating, {.f = std::stof(s)}};
  } else {
    if (!isSuffix(last)) 
      return {LiteralType::integer, {.i = std::stoi(s, 0, 10)}};
    switch (last) {
      case 'L':
        return {LiteralType::long_int, {.l = std::stoll(s, 0, 10)}};
      case 'b':
        return {LiteralType::binary, {.b = std::stoull(s, 0, 2)}};
      case 'f':
        return {LiteralType::floating, {.f = std::stof(s)}};
      case 'd':
        return {LiteralType::double_floating, {.d = std::stod(s)}};
      default:
        return {};
    }
  }
  return {};
}

std::string Literal::Literal::toString() {
  char buf[255];
  switch (this->type) {
    case LiteralType::binary :
      sprintf(buf, "%I64u", this->u.b);
      return "BINARY(" + std::string(buf) + ")";
    case LiteralType::character :
      buf[0] = this->u.c;
      buf[1] = '\0';
      return "CHARACTER(" + std::string(buf) + ")";
    case LiteralType::double_floating :
      sprintf(buf, "%f", this->u.d);
      return "DOUBLE(" + std::string(buf) + ")";
    case LiteralType::floating :
      sprintf(buf, "%f", this->u.f);
      return "FLOAT(" + std::string(buf) + ")";
    case LiteralType::hexadecimal :
      sprintf(buf, "%I64u", this->u.h);
      return "HEX(" + std::string(buf) + ")";
    case LiteralType::integer :
      sprintf(buf, "%d", this->u.i);
      return "INT(" + std::string(buf) + ")";
    case LiteralType::long_int :
      sprintf(buf, "%lld", this->u.l);
      return "LONG(" + std::string(buf) + ")";
    case LiteralType::string :
      return "STRING(" + std::string(this->u.s) + ")";
    default:
      return "NULL";
  }
}

bool Literal::Literal::operator==(Literal a) {
  if (this->type != a.type) return false;
  switch (this->type) {
    case LiteralType::binary :
      return this->u.b == a.u.b;
    case LiteralType::character :
      return this->u.c == a.u.c;
    case LiteralType::double_floating :
      return this->u.d == a.u.d;
    case LiteralType::floating :
      return this->u.f == a.u.f;
    case LiteralType::hexadecimal :
      return this->u.h == a.u.h;
    case LiteralType::integer :
      return this->u.i == a.u.i;
    case LiteralType::long_int :
      return this->u.l == a.u.l;
    case LiteralType::string :
      return std::string(this->u.s) == std::string(a.u.s);
    default:
      return false;
  }
}


