#include "Tokenizer.hpp"

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
        tokens.push_back({Tokens::TokenType::slash, string(), line});
      }
    } else if (tryconsume('*')) {
      if (tryconsume('/')) {
        multi_comment = false;
      } else {
        tokens.push_back({Tokens::TokenType::star, string(), line});
      }
    } else {
      stringstream buf;
      if (isalpha(peek())) {
        while (isalnum(peek()))
          buf << consume();
        if (0) {

        } else {
          tokens.push_back({Tokens::TokenType::identifier, buf.str(), line});
        }
      } else if (isdigit(peek())) {
        while (isdigit(peek()) || peek() == '.')
          buf << consume();
        if (StringUtils::isInString(peek(), string(LITERAL_LONG)+LITERAL_FLOAT+LITERAL_DOUBLE+LITERAL_BINARY+LITERAL_OCTAL+LITERAL_HEX))
          buf << consume();
        tokens.push_back({Tokens::TokenType::literal, buf.str(), line});
      } else if (!isspace(peek())) {
        while (!isspace(peek()))
          buf << consume();
        tokens.push_back({Tokens::TokenType::symbols, buf.str(), line});
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
