#include <Tokenizer/Tokenizer.hpp>
#include "Tokenizer.hpp"

Tokenizer::Tokenizer::Tokenizer(std::string s) {
  for (char c : s)
    content.push_back(c);
}

std::vector<Tokens::Token> Tokenizer::Tokenizer::tokenize() {
  using std::vector, std::string, std::stringstream;

  vector<Tokens::Token> tokens{};
  bool comment = false;
  bool multi_comment = false;
  
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
        tokens.push_back({Tokens::TokenType::symbols, line, "/"});
      }
    } else if (tryconsume('*')) {
      if (tryconsume('/')) {
        multi_comment = false;
      } else {
        tokens.push_back({Tokens::TokenType::symbols, line, "*"});
      }

    } else if (tryconsume('(')) {
      tokens.push_back({Tokens::TokenType::open_paren, line});
    } else if (tryconsume(')')) {
      tokens.push_back({Tokens::TokenType::close_paren, line});
    } else if (tryconsume('{')) {
      tokens.push_back({Tokens::TokenType::open_curly, line});
    } else if (tryconsume('}')) {
      tokens.push_back({Tokens::TokenType::close_curly, line});
    } else if (tryconsume('[')) {
      tokens.push_back({Tokens::TokenType::open_square, line});
    } else if (tryconsume(']')) {
      tokens.push_back({Tokens::TokenType::close_square, line});
    } else if (tryconsume('<')) {
      tokens.push_back({Tokens::TokenType::open_angle, line});
    } else if (tryconsume('>')) {
      tokens.push_back({Tokens::TokenType::close_angle, line});
    } else if (tryconsume(';')) {
      tokens.push_back({Tokens::TokenType::semicolon, line});
    } else if (tryconsume('@')) {
      tokens.push_back({Tokens::TokenType::at, line});
    } else if (tryconsume('$')) {
      tokens.push_back({Tokens::TokenType::preprocessor, line});
    } else if (tryconsume('.')) {
      if (peek() == '.' && peek(1) == '.') {
        tokens.push_back({Tokens::TokenType::ellipsis, line});
        consume(2);
      } else {
        tokens.push_back({Tokens::TokenType::dot, line});
      }
    } else if (tryconsume(',')) {
      tokens.push_back({Tokens::TokenType::comma, line});
    } else if (tryconsume('\'')) {
      char c = consume();
      if (!tryconsume('\''))
        Errors::error("Missing token", "Closing single quote expected", line);
      tokens.push_back({Tokens::TokenType::literal, line, StringUtils::parseEscapes("'" + string(1, c) + "'")});
    } else if (tryconsume('"')) {
      stringstream ss;
      ss << '"';
      while (hasPeek() && !tryconsume('"')) {
        if (peek() == '\\') {
          ss << consume();
        }
        ss << consume();
      }
      if (!hasPeek())
        Errors::error("Missing token", "Closing double quote expected", line);
      ss << '"';
      string s = StringUtils::parseEscapes(ss.str());
      tokens.push_back({Tokens::TokenType::literal, line, s});
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
          tokens.push_back({Tokens::TokenType::Byte, line});
        } else if (buffer == "string") {
          tokens.push_back({Tokens::TokenType::String, line});
        } else if (buffer == "void") {
          tokens.push_back({Tokens::TokenType::Void, line});
        } else if (buffer == "struct") {
          tokens.push_back({Tokens::TokenType::Struct, line});
        } else if (buffer == "union") {
          tokens.push_back({Tokens::TokenType::Union, line});
        } else if (buffer == "interface") {
          tokens.push_back({Tokens::TokenType::Interface, line});
        } else if (buffer == "return") {
          tokens.push_back({Tokens::TokenType::Return, line});
        } else if (buffer == "mutable") {
          tokens.push_back({Tokens::TokenType::Mutable, line});
        } else if (buffer == "unsigned") {
          tokens.push_back({Tokens::TokenType::Unsigned, line});
        } else if (buffer == "operator") {
          tokens.push_back({Tokens::TokenType::Operator, line});
        } else if (buffer == "unary") {
          tokens.push_back({Tokens::TokenType::Unary, line});
        } else if (buffer == "binary") {
          tokens.push_back({Tokens::TokenType::Binary, line});
        } else if (buffer == "type") {
          tokens.push_back({Tokens::TokenType::Type, line});
        } else if (buffer == "if") {
          tokens.push_back({Tokens::TokenType::If, line});
        } else if (buffer == "else") {
          tokens.push_back({Tokens::TokenType::Else, line});
        } else if (buffer == "while") {
          tokens.push_back({Tokens::TokenType::While, line});
        } else if (buffer == "do") {
          tokens.push_back({Tokens::TokenType::Do, line});
        } else if (buffer == "for") {
          tokens.push_back({Tokens::TokenType::For, line});
        } else if (buffer == "namespace") {
          tokens.push_back({Tokens::TokenType::Namespace, line});
        } else if (buffer == "defer") {
          tokens.push_back({Tokens::TokenType::Defer, line});
        } else if (buffer == "as") {
          tokens.push_back({Tokens::TokenType::As, line});
        } else if (buffer == "define") {
          tokens.push_back({Tokens::TokenType::define, line});
        } else if (buffer == "undefine") {
          tokens.push_back({Tokens::TokenType::undefine, line});
        } else if (buffer == "defined") {
          tokens.push_back({Tokens::TokenType::defined, line});
        } else if (buffer == "endif") {
          tokens.push_back({Tokens::TokenType::endif, line});
        } else if (buffer == "elseif") {
          tokens.push_back({Tokens::TokenType::elseif, line});
        } else if (buffer == "include") {
          tokens.push_back({Tokens::TokenType::include, line});
        } else if (buffer == "impl") {
          tokens.push_back({Tokens::TokenType::Impl, line});
        } else if (buffer == "asm") {
          while (tryconsume(' ') || tryconsume('\r'));
          while (tryconsume('\n')) {line++;};
          if (!tryconsume('{'))
            Errors::error("Missing token", "Opening curly bracket expected", line);
          stringstream ss;
          while (hasPeek() && !tryconsume('}')) {
            ss << consume();
          }
          if (!hasPeek())
            Errors::error("Missing token", "Closing curly bracket expected", line);
          tokens.push_back({Tokens::TokenType::Asm, line, ss.str()});
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
        while (!isalnum(peek()) && !isspace(peek()))
          buf << consume();
        tokens.push_back({Tokens::TokenType::symbols, line, buf.str()});
        buf.str("");
      } else {
        Errors::error("Invalid Token", Formatting::format("Token '%s' not recognized", string(1, peek())), line);
      }
    }
  }
  return tokens;
}

char Tokenizer::Tokenizer::null() {
  return 0;
}

int Tokenizer::Tokenizer::getCurrentLine() {
  return this->line;
}

bool Tokenizer::Tokenizer::equalCriteria(char a, char b) {
  return a == b;
}
