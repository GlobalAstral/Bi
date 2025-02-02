#pragma once 

#include <iostream>
#include <List.hpp>
#include <string.h>

namespace Assembly {
  struct Token {
    char* instruction;
    Lists::List<char*> params;
  };

  class AssemblyTokenizer {
    public:
      AssemblyTokenizer(std::string s) {
        this->content = s;
      }
      Lists::List<Token*>* parseAsm();
    private:
      std::string content;
      int _peek = 0;
      bool hasPeek() {
        return (_peek >= 0 && _peek < content.size());
      }
      char peek(int offset = 0) {
        if (!hasPeek() || _peek + offset >= content.size() || _peek + offset < 0) return 0;
        return content.at(_peek);
      }
      char consume() {
        if (!hasPeek()) return 0;
        return content.at(_peek++);
      }
      void ignoreSpaces() {
        while (hasPeek() && (peek() == ' '))
          consume();
      }
  };
}

