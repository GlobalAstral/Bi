#include <Preprocessor.hpp>

Lists::List<Tokens::Token*> Preprocessor::Preprocessor::preprocess() {
  Lists::List<Tokens::Token*> ret{};
  Lists::List<Definition> definitions{[](Definition A, Definition B){return A.name == B.name;}};

  while (_peek < content.size()) {
    if (try_consume(Tokens::TokenType::PREPROCESSOR)) {
      if (try_consume(Tokens::TokenType::DEFINE)) {
        if (peek()->type != Tokens::TokenType::IDENTIFIER) Errors::error("Expected Identifier", peek(-1)->line);
        Definition def;
        Tokens::Token* ident = consume();
        def.name = ident->value;
        if (try_consume(Tokens::TokenType::OPEN_ANGLE))
          while (peek()->type == Tokens::TokenType::IDENTIFIER) {
            def.params.push(consume());
            if (try_consume(Tokens::TokenType::CLOSE_ANGLE)) break;
            if (!try_consume(Tokens::TokenType::COMMA)) Errors::error("Comma expected between identifiers", peek(-1)->line);
          }
        while (!try_consume(Tokens::TokenType::PREPROCESSOR))
          def.content.push(consume());
        definitions.push(def);
      }
    } else if (peek()->type == Tokens::TokenType::IDENTIFIER) {
      Tokens::Token* ident = consume();
      if (!definitions.contains({ident->value, Lists::List<Tokens::Token*>{}, Lists::List<Tokens::Token*>{}}))
        continue;

      Definition def = definitions.at(definitions.index({ident->value, Lists::List<Tokens::Token*>{}, Lists::List<Tokens::Token*>{}}));
      Lists::List<Tokens::Token*> temp = def.content.copy();
      Dict::Dict<Tokens::Token*, Lists::List<Tokens::Token*>*> map{[](Tokens::Token* a, Tokens::Token* b){return (a->type == b->type) && (a->value == b->value);}};
      if (try_consume(Tokens::TokenType::OPEN_ANGLE)) {
        Lists::List<Tokens::Token*>* buf = new Lists::List<Tokens::Token*>{};
        int paramIndex = 0;
        while (!try_consume(Tokens::TokenType::CLOSE_ANGLE)) {
          if (try_consume(Tokens::TokenType::COMMA)) {
            Lists::List<Tokens::Token*> tmp = buf->copy();
            map.set(def.params.at(paramIndex), &tmp);
            std::cout << map.toString();
            paramIndex++;
            buf->reset();
          } else
            buf->push(consume());
        }
        Lists::List<Tokens::Token*> tmp = buf->copy();
        map.set(def.params.at(paramIndex), &tmp);
        std::cout << map.toString();
        paramIndex++;
        buf->reset();
        std::cout << map.toString();

        for (int i = 0; i < def.content.size(); i++) {
          if (def.content.at(i)->type != Tokens::TokenType::IDENTIFIER) continue;
          Tokens::Token* ident = def.content.at(i);
          if (!map.contains(ident)) continue;
          std::cout << map.toString();
          Lists::List<Tokens::Token*>* replace = map.get(ident);
          temp.pop(i);
          for (int j = replace->last(); j >= 0; j--)
            temp.insert(replace->at(j), i);
        }
    }
    
      for (int i = 0; i < temp.size(); i++)
        ret.push(temp.at(i));
    } else {
      ret.push(consume());
    }
  }
  return ret;
}
