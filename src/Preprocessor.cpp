#include <Preprocessor.hpp>

Preprocessor::Definition* nominativeDef(std::string name) {
  return new Preprocessor::Definition{name, Lists::List<Tokens::Token*>{}, Lists::List<Tokens::Token*>{}};
}

Preprocessor::Definition* Preprocessor::Preprocessor::preprocessDefine(Tokens::Token* ident) {
  Definition* def = new Definition{};
  
  def->name = ident->value;
  if (try_consume(Tokens::TokenType::OPEN_ANGLE))
    while (peek()->type == Tokens::TokenType::IDENTIFIER) {
      def->params.push(consume());
      if (try_consume(Tokens::TokenType::CLOSE_ANGLE)) break;
      if (!try_consume(Tokens::TokenType::COMMA)) Errors::error("Comma expected between identifiers", peek(-1)->line);
    }
  bool notFound = false;
  while ((notFound = !try_consume(Tokens::TokenType::PREPROCESSOR))) {
    if (_peek == this->content.size()-1) break;
    def->content.push(consume());
  }
  if (notFound) Errors::error("Expected '#'");
  return def;
}

bool Preprocessor::Preprocessor::isComputable(Lists::List<Definition*>& definitions) {
  int prev = _peek;
  if (compute(definitions)->type == Tokens::TokenType::NULL_TOKEN) {
    _peek = prev;
    return false;
  }
  _peek = prev;
  return true;
}

Tokens::Token* Preprocessor::Preprocessor::compute(Lists::List<Definition*>& definitions) {
  if (try_consume(Tokens::TokenType::NOT)) {
    Tokens::Token* tok = compute(definitions);
    std::string val = (tok->value == "0") ? "1" : "0";
    return new Tokens::Token{Tokens::TokenType::NUMBER, tok->line, val};
  } else if (try_consume(Tokens::TokenType::DEFINED)) {
    if (peek()->type != Tokens::TokenType::IDENTIFIER) Errors::error("Expected Identifier");
    Tokens::Token* ident = consume();
    std::string val = (definitions.contains(nominativeDef(ident->value))) ? "1" : "0";
    return new Tokens::Token{Tokens::TokenType::NUMBER, ident->line, val};
  }
  return new Tokens::Token{Tokens::TokenType::NULL_TOKEN, -1, ""};
}

void Preprocessor::Preprocessor::preprocess(Lists::List<Tokens::Token*>& ret, Lists::List<Definition*>& definitions) {
  if (try_consume(Tokens::TokenType::PREPROCESSOR)) {
    if (try_consume(Tokens::TokenType::DEFINE)) {
      if (peek()->type != Tokens::TokenType::IDENTIFIER) Errors::error("Expected Identifier", peek(-1)->line);
      Tokens::Token* ident = consume();
      if (definitions.contains(nominativeDef(ident->value)))
        Errors::error("Definition already exists");
      definitions.push(preprocessDefine(ident));
    } else if (try_consume(Tokens::TokenType::UNDEFINE)) {
      if (peek()->type != Tokens::TokenType::IDENTIFIER) Errors::error("Expected Identifier", peek(-1)->line);
      Tokens::Token* ident = consume();
      if (!definitions.contains(nominativeDef(ident->value)))
        Errors::error("Definition does not exists");
      definitions.pop(definitions.index(nominativeDef(ident->value)));
    } else if (try_consume(Tokens::TokenType::IF)) {
      if (!isComputable(definitions)) Errors::error("Expected Preprocessor Expression");
      std::string val = compute(definitions)->value;
      bool notFound = false;
      bool ignore = val == "0";
      if (!try_consume(Tokens::TokenType::OPEN_BRACKET)) Errors::error("Expected open bracket");
      while ((notFound = !try_consume(Tokens::TokenType::CLOSE_BRACKET))) {
        if (_peek == this->content.size()-1) break;
        if (ignore)
          consume();
        else
          preprocess(ret, definitions);
      }
    }
  } else if (peek()->type == Tokens::TokenType::IDENTIFIER) {
    Tokens::Token* ident = consume();
    if (!definitions.contains(nominativeDef(ident->value))) {
      ret.push(ident);
      return;
    }

    Definition* def = definitions.at(definitions.index(nominativeDef(ident->value)));
    Lists::List<Tokens::Token*> temp = def->content.copy();
    Dict::Dict<Tokens::Token*, Lists::List<Tokens::Token*>*> map{[](Tokens::Token* a, Tokens::Token* b){return (a->type == b->type) && (a->value == b->value);}};
    if (try_consume(Tokens::TokenType::OPEN_ANGLE)) {
      Lists::List<Tokens::Token*>* buf = new Lists::List<Tokens::Token*>{};
      int paramIndex = 0;
      while (!try_consume(Tokens::TokenType::CLOSE_ANGLE)) {
        if (try_consume(Tokens::TokenType::COMMA)) {
          Lists::List<Tokens::Token*> tmp = buf->copy();
          map.set(def->params.at(paramIndex), &tmp);
          paramIndex++;
          buf->reset();
        } else
          buf->push(consume());
      }
      Lists::List<Tokens::Token*> tmp = buf->copy();
      map.set(def->params.at(paramIndex), &tmp);
      paramIndex++;
      buf->reset();

      for (int i = 0; i < def->content.size(); i++) {
        if (def->content.at(i)->type != Tokens::TokenType::IDENTIFIER) continue;
        Tokens::Token* ident = def->content.at(i);
        if (!map.contains(ident)) continue;
        Lists::List<Tokens::Token*>* replace = map.get(ident);
        temp.pop(i);
        for (int j = replace->last(); j >= 0; j--)
          temp.insert(replace->at(j), i);
      }
  }
  
    for (int i = 0; i < temp.size(); i++)
      ret.push(temp.at(i));
  } else if (isComputable(definitions)) {
    ret.push(compute(definitions));
  } else {
    ret.push(consume());
  }
}

Lists::List<Tokens::Token*> Preprocessor::Preprocessor::preprocessAll() {
  Lists::List<Tokens::Token*> ret{};
  Lists::List<Definition*> definitions = {[](Definition* A, Definition* B){return A->name == B->name;}};

  while (_peek < content.size()) {
    preprocess(ret, definitions);
  }
  return ret;
}
