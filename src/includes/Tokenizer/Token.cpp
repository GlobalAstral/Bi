#include <Tokenizer/Token.hpp>
#include "Token.hpp"


std::string getTypeAsString(Tokens::TokenType type) {
  switch (type) {
    case Tokens::TokenType::open_paren: return "OPEN_PAREN";
    case Tokens::TokenType::close_paren: return "CLOSE_PAREN";
    case Tokens::TokenType::open_curly: return "OPEN_CURLY";
    case Tokens::TokenType::close_curly: return "CLOSE_CURLY";
    case Tokens::TokenType::open_angle: return "OPEN_ANGLE";
    case Tokens::TokenType::close_angle: return "CLOSE_ANGLE";
    case Tokens::TokenType::open_square: return "OPEN_SQUARE";
    case Tokens::TokenType::close_square: return "CLOSE_SQUARE";
    case Tokens::TokenType::semicolon: return "SEMICOLON";
    case Tokens::TokenType::literal: return "LITERAL";
    case Tokens::TokenType::symbols: return "SYMBOLS";
    case Tokens::TokenType::identifier: return "IDENTIFIER";
    case Tokens::TokenType::Int: return "INT";
    case Tokens::TokenType::Uint: return "UINT";
    case Tokens::TokenType::Float: return "FLOAT";
    case Tokens::TokenType::Long: return "LONG";
    case Tokens::TokenType::Ulong: return "ULONG";
    case Tokens::TokenType::Double: return "DOUBLE";
    case Tokens::TokenType::Char: return "CHAR";
    case Tokens::TokenType::Byte: return "BYTE";
    case Tokens::TokenType::String: return "STRING";
    case Tokens::TokenType::Void: return "VOID";
    case Tokens::TokenType::Struct: return "STRUCT";
    case Tokens::TokenType::Union: return "UNION";
    case Tokens::TokenType::Interface: return "INTERFACE";
    case Tokens::TokenType::Return: return "RETURN";
    case Tokens::TokenType::Mutable: return "MUTABLE";
    case Tokens::TokenType::Unary: return "UNARY";
    case Tokens::TokenType::Binary: return "BINARY";
    case Tokens::TokenType::Type: return "TYPE";
    case Tokens::TokenType::Asm: return "ASM";
    case Tokens::TokenType::If: return "IF";
    case Tokens::TokenType::Else: return "ELSE";
    case Tokens::TokenType::While: return "WHILE";
    case Tokens::TokenType::Do: return "DO";
    case Tokens::TokenType::For: return "FOR";
    case Tokens::TokenType::Namespace: return "NAMESPACE";
    case Tokens::TokenType::Defer: return "DEFER";
    case Tokens::TokenType::preprocessor: return "PREPROCESSOR";
    case Tokens::TokenType::As: return "AS";
    case Tokens::TokenType::dot: return "DOT";
    case Tokens::TokenType::comma: return "COMMA";
    case Tokens::TokenType::define: return "DEFINE";
    case Tokens::TokenType::undefine: return "UNDEFINE";
    case Tokens::TokenType::defined: return "DEFINED";
    case Tokens::TokenType::endif: return "ENDIF";
    case Tokens::TokenType::elseif: return "ELSEIF";
    case Tokens::TokenType::include: return "INCLUDE";
    case Tokens::TokenType::Impl: return "IMPL";
    case Tokens::TokenType::ellipsis: return "ELLIPSIS";
    case Tokens::TokenType::arrow: return "ARROW";
    case Tokens::TokenType::colon: return "COLON";
    case Tokens::TokenType::method: return "METHOD";
    case Tokens::TokenType::Inline: return "INLINE";
    default: return "NULL";
  }
}

std::string Tokens::Token::toString() {
  using std::string, std::stringstream;
  string typeAsString = getTypeAsString(this->type);
  
  string ret = Formatting::format("%s(\"%s\")<%d>", typeAsString.c_str(), this->value.c_str(), this->line);
  return ret;
}

Tokens::Token Tokens::nullToken() {
  return {Tokens::TokenType::null, 0, ""};
}
