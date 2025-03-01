#pragma once

#include <iostream>
#include <fstream>
#include <Tokenizer.hpp>
#include <Errors.hpp>
#include <Dict.hpp>
namespace Preprocessor {

  typedef struct {
    std::string name;
    Lists::List<Tokens::Token*> content;
    Lists::List<Tokens::Token*> params;
  } Definition;

  class Preprocessor {
    public:
      Preprocessor(Lists::List<Tokens::Token*> tokens) {
        this->content = tokens;
      }
      Lists::List<Tokens::Token*> preprocessAll();
      void preprocess(Lists::List<Tokens::Token*>& ret, Lists::List<Definition*>& definitions);
      Definition* preprocessDefine(Tokens::Token* ident, Lists::List<Definition*>& definitions);
      void preprocessIdentifier(Lists::List<Tokens::Token*>& ret, Lists::List<Definition*>& definitions);
      Tokens::Token* compute(Lists::List<Definition*>& definitions);
      bool isComputable(Lists::List<Definition*>& definitions);
    private:
      int _peek = 0;
      Lists::List<Tokens::Token*> content;
      Tokens::Token* peek(int offset = 0) {
        return content.at(_peek + offset);
      }
      Tokens::Token* consume() {
        return content.at(_peek++);
      }
      bool hasPeek() {
        return (_peek < content.size());
      }
      bool try_consume(Tokens::TokenType type) {
        if (hasPeek() && peek()->type == type) {
          consume();
          return true;
        }
        return false;
      }
  };
}
