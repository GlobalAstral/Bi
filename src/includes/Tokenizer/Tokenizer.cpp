#include <Tokenizer/Tokenizer.hpp>

Tokenizer::Tokenizer::Tokenizer(std::string s) {
  this->content = std::string(s);
}

std::vector<Tokens::Token> Tokenizer::Tokenizer::tokenize() {
  using std::vector, std::string, std::stringstream;
  
  vector<Tokens::Token> tokens{};
  bool comment = false;
  bool multi_comment = false;
  unsigned int line = 1;
  while (hasPeek()) {
    if (comment || multi_comment || peek() == ' ' || peek() == '\r') {
      consume();
    } else if (peek() == '\n') {
      consume();
      comment = false;
      line++;
    } else if (tryconsume('/')) {
      if (tryconsume('/')) {
        comment = true;
      } else if (tryconsume('*')) {
        multi_comment = true;
      } else {
        tokens.push_back({Tokens::TokenType::slash, line});
      }
    } else if (tryconsume('*')) {
      if (tryconsume('/')) {
        multi_comment = false;
      } else {
        tokens.push_back({Tokens::TokenType::star, line});
      }

    } else if (tryconsume('(')) {
      tokens.push_back({Tokens::TokenType::open_paren, line});
    } else if (tryconsume(')')) {
      tokens.push_back({Tokens::TokenType::close_paren, line});
    } else if (tryconsume('{')) {
      tokens.push_back({Tokens::TokenType::open_curly, line});
    } else if (tryconsume('}')) {
      tokens.push_back({Tokens::TokenType::close_curly, line});
    } else if (tryconsume(';')) {
      tokens.push_back({Tokens::TokenType::semicolon, line});
    } else {
      stringstream buf;
      if (isalpha(peek())) {
        while (isalnum(peek()))
          buf << consume();
        string buffer = string(buf.str());
        if (buffer == "int") {
          tokens.push_back({Tokens::TokenType::Int, line});
        } else if (buffer == "float") {
          tokens.push_back({Tokens::TokenType::Float, line});
        } else if (buffer == "long") {
          tokens.push_back({Tokens::TokenType::Long, line});
        } else if (buffer == "double") {
          tokens.push_back({Tokens::TokenType::Double, line});
        } else if (buffer == "char") {
          tokens.push_back({Tokens::TokenType::Char, line});
        } else if (buffer == "byte") {
          tokens.push_back({Tokens::TokenType::Byte, line});tokens.push_back({Tokens::TokenType::Int, line});
        } else if (buffer == "string") {
          tokens.push_back({Tokens::TokenType::String, line});
        } else if (buffer == "void") {
          tokens.push_back({Tokens::TokenType::Void, line});
        } else {
          tokens.push_back({Tokens::TokenType::identifier, line, buf.str()});
        }
        buf.str("");
      } else if (isdigit(peek())) {
        while (isdigit(peek()) || peek() == '.')
          buf << consume();
        if (StringUtils::isInString(peek(), string(LITERAL_LONG)+LITERAL_FLOAT+LITERAL_DOUBLE+LITERAL_BINARY+LITERAL_OCTAL+LITERAL_HEX))
          buf << consume();
        tokens.push_back({Tokens::TokenType::literal, line, buf.str()});
        buf.str("");
      } else if (!isspace(peek())) {
        while (!isspace(peek()))
          buf << consume();
        tokens.push_back({Tokens::TokenType::symbols, line, buf.str()});
        buf.str("");
      } else {
        Errors::error("Invalid Token", Formatting::format("Token '%s' not recognized", ("" + peek())), line);
      }
    }
  }
  return tokens;
}

bool Tokenizer::Tokenizer::hasPeek(int offset) {
  return this->_peek+offset >= 0 && this->_peek+offset < this->content.size();
}

char Tokenizer::Tokenizer::peek(int offset) {
  if (hasPeek(offset))
    return this->content[_peek+offset];
  return 0;
}

char Tokenizer::Tokenizer::consume() {
  if (hasPeek())
    return content[_peek++];
  return 0;
}

bool Tokenizer::Tokenizer::tryconsume(char c) {
  if (hasPeek() && peek() == c) {
    consume();
    return true;
  }
  return false;
}
