#include <AssemblyTokenizer.hpp>

Lists::List<Assembly::Token*>* Assembly::AssemblyTokenizer::parseAsm() {
  Lists::List<Assembly::Token*>* ret = new Lists::List<Assembly::Token*>{};

  while (hasPeek()) {
    if (peek() == ' ') {
      consume();
    } else {
      std::string buffer = "";
      while (isalnum(peek())) {
        buffer += consume();
      }
      char* instruction = const_cast<char*>(buffer.c_str());
      Lists::List<char*> params{};
      this->ignoreSpaces();
      while (peek() != '\n' && peek() != 0) {
        std::string buf = "";
        while (isalnum(peek())) {
          buf += consume();
        }
        this->ignoreSpaces();
        if (peek() == ',') consume();
        this->ignoreSpaces();
        params.push(const_cast<char*>(buf.c_str()));
      }
      consume();
      ret->push(new Assembly::Token{instruction, params});
    }
  }

  return ret;
}
