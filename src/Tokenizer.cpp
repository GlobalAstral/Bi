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
    case TokenType::INTEGER:
      ss << "INTEGER";
      break;
    case TokenType::FLOATING:
      ss << "FLOATING";
      break;
    case TokenType::DOUBLE:
      ss << "DOUBLE";
      break;
    case TokenType::LONG:
      ss << "LONG";
      break;
    case TokenType::CHARACTER:
      ss << "CHAR";
      break;
    case TokenType::STRING:
      ss << "STRING";
      break;
    case TokenType::BINARY:
      ss << "BINARY";
      break;
    case TokenType::HEXADECIMAL:
      ss << "HEX";
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
    default:
      ss << "NULL";
      break;
  }
  ss << " with value '";
  ss << value;
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
    } else if (try_consume('\'')) {
      char buf[1] = {consume()};
      tokens.push(new Tokens::Token{Tokens::TokenType::CHARACTER, line, std::string(buf)});
      if (!try_consume('\'')) Errors::error("Expected closing single quote");
    } else if (try_consume('"')) {
      std::string buf = "";
      bool notFound = false;
      while ((notFound = !try_consume('"')))
        buf += consume();
      if (notFound) Errors::error("Expected closing quote");
      tokens.push(new Tokens::Token{Tokens::TokenType::STRING, line, buf});
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
        } else {
          tokens.push(new Tokens::Token{Tokens::TokenType::IDENTIFIER, line, std::string(buf)});
        }
      } else {
        Tokens::TokenType tok = TokenType::INTEGER;
        std::string buf = "";
        bool hex = false;
        if (peek() == '0') {
          buf += consume();
          if (peek() == 'x') {
            buf += consume();
            hex = true;
            tok = TokenType::HEXADECIMAL;
          }
        }
        while (isdigit(peek())) {
          buf += consume();
        }
        
        if (peek() == '.') {
          if (hex) Errors::error("Cannot have float hexadecimal literal");
          buf += consume();
          while (isdigit(peek())) {
            buf += consume();
          }
          tok = TokenType::DOUBLE;
          if (try_consume('f')) tok = TokenType::FLOATING;
          if (try_consume('L') || try_consume('b')) Errors::error("Cannot have long or binary float literal");
        } else {
          if (try_consume('L'))
            tok = TokenType::LONG;
          else if (try_consume('b'))
            tok = TokenType::BINARY;
          else if (try_consume('d'))
            tok = TokenType::DOUBLE;
          else if (try_consume('f'))
            tok = TokenType::FLOATING;
        }
        tokens.push(new Tokens::Token{tok, line, std::string(buf)});
      }
    }
  }
  return tokens;
}
