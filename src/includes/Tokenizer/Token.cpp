#include <Tokenizer/Token.hpp>


std::string getTypeAsString(Tokens::TokenType type) {
  switch (type) {
    case Tokens::TokenType::star: return "STAR";
    case Tokens::TokenType::slash: return "SLASH";
    case Tokens::TokenType::open_paren: return "OPEN_PAREN";
    case Tokens::TokenType::close_paren: return "CLOSE_PAREN";
    case Tokens::TokenType::open_curly: return "OPEN_CURLY";
    case Tokens::TokenType::close_curly: return "CLOSE_CURLY";
    case Tokens::TokenType::semicolon: return "SEMICOLON";
    case Tokens::TokenType::literal: return "LITERAL";
    case Tokens::TokenType::symbols: return "SYMBOLS";
    case Tokens::TokenType::identifier: return "IDENTIFIER";
    case Tokens::TokenType::Int: return "INT";
    case Tokens::TokenType::Float: return "FLOAT";
    case Tokens::TokenType::Long: return "LONG";
    case Tokens::TokenType::Double: return "DOUBLE";
    case Tokens::TokenType::Char: return "CHAR";
    case Tokens::TokenType::Byte: return "BYTE";
    case Tokens::TokenType::String: return "STRING";
    case Tokens::TokenType::Void: return "VOID";
    case Tokens::TokenType::Return: return "RETURN";
    default: return "NULL";
  }
}

std::string Tokens::Token::toString() {
  using std::string, std::stringstream;
  string typeAsString = getTypeAsString(this->type);
  
  string ret = Formatting::format("%s(\"%s\")<%d>", typeAsString.c_str(), this->value.c_str(), this->line);
  return ret;
}