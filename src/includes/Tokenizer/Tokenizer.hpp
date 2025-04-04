#pragma once
#include <iostream>
#include <vector>
#include <Tokenizer/Token.hpp>
#include <format>
#include <Utils/Errors.hpp>
#include <Utils/Formatting.hpp>
#include <Utils/StringUtils.hpp>
#include <Utils/Constants.hpp>

namespace Tokenizer {
  class Tokenizer {
    public:
      Tokenizer(std::string s);
      std::vector<Tokens::Token> tokenize();
    private:
      bool hasPeek(int offset = 0);
      char peek(int offset = 0);
      char consume();
      bool tryconsume(char c);
      std::string content;
      int _peek = 0;
  };
}
