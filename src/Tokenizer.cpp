#include <Tokenizer.hpp>

std::string Tokens::Token::toString() {
  std::stringstream ss;
  ss << "Token of type ";
  switch (type) {
    case TokenType::PREPROCESSOR:
      ss << "PREPROCESSOR";
      break;
    case TokenType::MEMBOX:
      ss << "MEMBOX";
      break;
    case TokenType::DEFINE:
      ss << "DEFINE";
      break;
    case TokenType::IDENTIFIER:
      ss << "IDENTIFIER";
      break;
    case TokenType::LITERAL:
      ss << "LITERAL";
      break;
    case TokenType::OPEN_PAREN:
      ss << "OPEN_PAREN";
      break;
    case TokenType::CLOSE_PAREN:
      ss << "CLOSE_PAREN";
      break;
    case TokenType::COMMA:
      ss << "COMMA";
      break;
    case TokenType::OPEN_ANGLE:
      ss << "OPEN_ANGLE";
      break;
    case TokenType::CLOSE_ANGLE:
      ss << "CLOSE_ANGLE";
      break;
    case TokenType::OPEN_SQUARE:
      ss << "OPEN_SQUARE";
      break;
    case TokenType::CLOSE_SQUARE:
      ss << "CLOSE_SQUARE";
      break;
    case TokenType::OPEN_BRACKET:
      ss << "OPEN_BRACKET";
      break;
    case TokenType::CLOSE_BRACKET:
      ss << "CLOSE_BRACKET";
      break;
    case TokenType::UNDEFINE:
      ss << "UNDEFINE";
      break;
    case TokenType::DEFINED:
      ss << "DEFINED";
      break;
    case TokenType::NOT:
      ss << "NOT";
      break;
    case TokenType::IF:
      ss << "IF";
      break;
    case TokenType::LABEL :
      ss << "LABEL";
      break;
    case TokenType::PUBLIC :
      ss << "PUBLIC";
      break;
    case TokenType::METHOD :
      ss << "METHOD";
      break;
    case TokenType::SEMICOLON :
      ss << "SEMICOLON";
      break;
    case TokenType::STRUCT :
      ss << "STRUCT";
      break;
    case TokenType::UNION :
      ss << "UNION";
      break;
    default:
      ss << "NULL";
      break;
  }
  ss << " with value '";
  if (this->type == TokenType::LITERAL)
    ss << this->value.lit.toString();
  else if (this->type == TokenType::IDENTIFIER)
    ss << this->value.identifier;

  ss << "'";
  ss << " at line ";
  ss << line;
  return ss.str();
}

Lists::List<Tokens::Token*> Tokens::Tokenizer::tokenize() {
  bool comment = false;
  int line = 1;
  Lists::List<Tokens::Token*> tokens{};
  while (peek() != '\0') {
    if (peek() == ' ' || comment) {
      consume();
    } else if (try_consume('\n')) {
      comment = false;
      line++;
    } else if (try_consume('/')) {
      if (try_consume('/'))
        comment = true;
    } else if (try_consume('#')) {
      tokens.push(new Tokens::Token{Tokens::TokenType::PREPROCESSOR, line});
    } else if (try_consume('(')) {
      tokens.push(new Tokens::Token{Tokens::TokenType::OPEN_PAREN, line});
    }  else if (try_consume(',')) {
      tokens.push(new Tokens::Token{Tokens::TokenType::COMMA, line});
    } else if (try_consume(')')) {
      tokens.push(new Tokens::Token{Tokens::TokenType::CLOSE_PAREN, line});
    } else if (try_consume('>')) {
      tokens.push(new Tokens::Token{Tokens::TokenType::GREATER, line});
    } else if (try_consume('<')) {
      tokens.push(new Tokens::Token{Tokens::TokenType::LESS, line});
    } else if (try_consume('[')) {
      tokens.push(new Tokens::Token{Tokens::TokenType::OPEN_SQUARE, line});
    } else if (try_consume(']')) {
      tokens.push(new Tokens::Token{Tokens::TokenType::CLOSE_SQUARE, line});
    } else if (try_consume('{')) {
      tokens.push(new Tokens::Token{Tokens::TokenType::OPEN_BRACKET, line});
    } else if (try_consume('}')) {
      tokens.push(new Tokens::Token{Tokens::TokenType::CLOSE_BRACKET, line});
    } else if (try_consume(';')) {
      tokens.push(new Tokens::Token{Tokens::TokenType::SEMICOLON, line});
    } else if (try_consume('\'')) {
      tokens.push(new Tokens::Token{Tokens::TokenType::LITERAL, line, {.lit = {Literal::LiteralType::character, {.c = consume()}}}});
      if (!try_consume('\'')) Errors::error("Expected closing single quote");
    } else if (try_consume('"')) {
      std::string buf = "";
      bool notFound = false;
      while ((notFound = !try_consume('"')))
        buf += consume();
      if (notFound) Errors::error("Expected closing quote");
      tokens.push(new Tokens::Token{Tokens::TokenType::LITERAL, line, {.lit = {Literal::LiteralType::string, {.s = const_cast<char*>(buf.c_str())}}}});
    } else {
      if (isalpha(peek())) {
        std::string buf = "";
        while (isalnum(peek())) {
          buf += consume();
        }

        if (buf == "membox") {
          tokens.push(new Tokens::Token{Tokens::TokenType::MEMBOX, line});
        } else if (buf == "define") {
          tokens.push(new Tokens::Token{Tokens::TokenType::DEFINE, line});
        } else if (buf == "undefine") {
          tokens.push(new Tokens::Token{Tokens::TokenType::UNDEFINE, line});
        } else if (buf == "defined") {
          tokens.push(new Tokens::Token{Tokens::TokenType::DEFINED, line});
        } else if (buf == "not") {
          tokens.push(new Tokens::Token{Tokens::TokenType::NOT, line});
        } else if (buf == "if") {
          tokens.push(new Tokens::Token{Tokens::TokenType::IF, line});
        } else if (buf == "label") {
          tokens.push(new Tokens::Token{Tokens::TokenType::LABEL, line});
        } else if (buf == "method") {
          tokens.push(new Tokens::Token{Tokens::TokenType::METHOD, line});
        } else if (buf == "public") {
          tokens.push(new Tokens::Token{Tokens::TokenType::PUBLIC, line});
        } else if (buf == "struct") {
          tokens.push(new Tokens::Token{Tokens::TokenType::STRUCT, line});
        } else if (buf == "union") {
          tokens.push(new Tokens::Token{Tokens::TokenType::UNION, line});
        } else {
          char* buffer = (char*)malloc(buf.size());
          strcpy(buffer, const_cast<char*>(std::string(buf).c_str()));
          tokens.push(new Tokens::Token{Tokens::TokenType::IDENTIFIER, line, {.identifier = buffer}});
        }
      } else {
        std::string buf = "";
        bool hex = false;
        if (isdigit(peek()))
          while (isalnum(peek())) {
            buf += consume();
          }
        
        if (peek() == '.') {
          if (hex) Errors::error("Cannot have float hexadecimal literal");
          buf += consume();
          if (isdigit(peek()))
            while (isalnum(peek())) {
              buf += consume();
            }
        }
        tokens.push(new Tokens::Token{Tokens::TokenType::LITERAL, line, {.lit = Literal::parseLiteral(std::string(buf))}});
      }
    }
  }
  return tokens;
}
