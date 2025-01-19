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
      tokens.push(new Tokens::Token{Tokens::TokenType::PREPROCESSOR, line, ""});
    } else if (try_consume('(')) {
      tokens.push(new Tokens::Token{Tokens::TokenType::OPEN_PAREN, line, ""});
    }  else if (try_consume(',')) {
      tokens.push(new Tokens::Token{Tokens::TokenType::COMMA, line, ""});
    } else if (try_consume(')')) {
      tokens.push(new Tokens::Token{Tokens::TokenType::CLOSE_PAREN, line, ""});
    } else if (try_consume('>')) {
      tokens.push(new Tokens::Token{Tokens::TokenType::GREATER, line, ""});
    } else if (try_consume('<')) {
      tokens.push(new Tokens::Token{Tokens::TokenType::LESS, line, ""});
    } else {
      if (isalpha(peek())) {
        std::string buf = "";
        while (isalnum(peek())) {
          buf += consume();
        }

        if (buf == "membox") {
          tokens.push(new Tokens::Token{Tokens::TokenType::MEMBOX, line, ""});
        } else if (buf == "define") {
          tokens.push(new Tokens::Token{Tokens::TokenType::DEFINE, line, ""});
        } else {
          tokens.push(new Tokens::Token{Tokens::TokenType::IDENTIFIER, line, std::string(buf)});
        }
      } else {
        Tokens::TokenType tok = TokenType::INTEGER;
        std::string buf = "";
        while (isdigit(peek())) {
          buf += consume();
        }
        
        if (peek() == '.') {
          buf += consume();
          while (isdigit(peek())) {
            buf += consume();
          }
          tok = TokenType::FLOATING;
        }
        tokens.push(new Tokens::Token{tok, line, std::string(buf)});
      }
    }
  }
  return tokens;
}
