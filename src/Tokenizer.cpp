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
    case TokenType::ASM :
      ss << "ASSEMBLY";
      break;
    case TokenType::SYMBOLS :
      ss << "SYMBOLS";
      break;
    case TokenType::OPERATION :
      ss << "OPERATION";
      break;
    case TokenType::TYPE :
      ss << "TYPE";
      break;
    case TokenType::BITS :
      ss << "BITS";
      break;
    case TokenType::SIMD :
      ss << "SIMD";
      break;
    default:
      ss << "NULL";
      break;
  }
  ss << " with value '";
  if (this->type == TokenType::LITERAL)
    ss << this->value.lit.toString();
  else if (this->type == TokenType::IDENTIFIER)
    ss << this->value.buffer;
  else if (this->type == TokenType::ASM)
    ss << this->value.assemblyCode->size();
  ss << "'";
  ss << " at line ";
  ss << line;
  return ss.str();
}

bool Tokens::isChar(char c, std::string s) {
  for (char ch : s) {
    if (c == ch)
      return true;
  }
  return false;
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
      tokens.push(new Tokens::Token{Tokens::TokenType::CLOSE_ANGLE, line});
    } else if (try_consume('<')) {
      tokens.push(new Tokens::Token{Tokens::TokenType::OPEN_ANGLE, line});
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
      if (!try_consume('\'')) Errors::error("Expected closing single quote", line);
    } else if (try_consume('"')) {
      std::string buf = "";
      bool notFound = false;
      while ((notFound = !try_consume('"')))
        buf += consume();
      if (notFound) Errors::error("Expected closing quote", line);
      char* buffer = (char*)malloc(buf.size());
      strcpy(buffer, buf.c_str());
      tokens.push(new Tokens::Token{Tokens::TokenType::LITERAL, line, {.lit = {Literal::LiteralType::string, {.s = buffer}}}});
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
        } else if (buf == "inline") {
          tokens.push(new Tokens::Token{Tokens::TokenType::INLINE, line});
        } else if (buf == "operation") {
          tokens.push(new Tokens::Token{Tokens::TokenType::OPERATION, line});
        } else if (buf == "bits") {
          tokens.push(new Tokens::Token{Tokens::TokenType::BITS, line});
        } else if (buf == "simd") {
          tokens.push(new Tokens::Token{Tokens::TokenType::SIMD, line});
        } else if (buf == "asm") {
          while (peek() == ' ' || peek() == '\n') 
            consume();
          if (!try_consume('{')) Errors::error("Expected '{'", line);
          std::string buff = "";
          bool notFound = false;
          while (peek() == ' ' || peek() == '\n') 
            consume();
          while ((notFound = !try_consume('}')))
            buff += consume();
          if (notFound) Errors::error("Expected '}'", line);
          Assembly::AssemblyTokenizer asmTokenizer{buff};
          Lists::List<Assembly::Token*>* code = asmTokenizer.parseAsm();
          tokens.push(new Tokens::Token{Tokens::TokenType::ASM, line, {.assemblyCode = code}});
        } else if (buf == "type") {
          tokens.push(new Tokens::Token{Tokens::TokenType::TYPE, line});
        } else {
          char* buffer = (char*)malloc(buf.size());
          strcpy(buffer, const_cast<char*>(std::string(buf).c_str()));
          tokens.push(new Tokens::Token{Tokens::TokenType::IDENTIFIER, line, {.buffer = buffer}});
        }
      } else if (!isalpha(peek()) && !isdigit(peek()) && !isChar(peek(), " \n;#(),<>[]{}")) {
        std::string buf = "";
        while (!isalpha(peek()) && !isdigit(peek()) && !isChar(peek(), " \n;#(),<>[]{}")) {
          buf += consume();
        }
        char* buffer = (char*)malloc(buf.size());
        strcpy(buffer, const_cast<char*>(std::string(buf).c_str()));
        tokens.push(new Tokens::Token{Tokens::TokenType::SYMBOLS, line, {.buffer = buffer}});
      } else {
        std::string buf = "";
        bool hex = false;
        if (isdigit(peek()))
          while (isalnum(peek())) {
            buf += consume();
          }
        
        if (peek() == '.') {
          if (hex) Errors::error("Cannot have float hexadecimal literal", line);
          buf += consume();
          if (isdigit(peek()))
            while (isalnum(peek())) {
              buf += consume();
            }
        }
        tokens.push(new Tokens::Token{Tokens::TokenType::LITERAL, line, {.lit = Literal::parseLiteral(std::string(buf), line)}});
      }
    }
  }
  return tokens;
}
