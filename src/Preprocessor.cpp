#include <Preprocessor.hpp>

Preprocessor::Definition* nominativeDef(std::string name) {
  return new Preprocessor::Definition{name, Lists::List<Tokens::Token*>{}, Lists::List<Tokens::Token*>{}};
}

Preprocessor::Definition* Preprocessor::Preprocessor::preprocessDefine(Tokens::Token* ident) {
  Definition* def = new Definition{};
  
  def->name = std::string(ident->value.buffer);
  if (peek()->type == Tokens::TokenType::SYMBOLS) {
    if (std::string(consume()->value.buffer) != "<")
      Errors::error("Expected '<'", peek(-1)->line);
    while (peek()->type == Tokens::TokenType::IDENTIFIER) {
      def->params.push(consume());
      if (peek()->type == Tokens::TokenType::SYMBOLS && std::string(consume()->value.buffer) == ">") break;
      if (!try_consume(Tokens::TokenType::COMMA)) Errors::error("Comma expected between identifiers", peek(-1)->line);
    }
  }
  bool notFound = false;
  while ((notFound = !try_consume(Tokens::TokenType::PREPROCESSOR))) {
    if (_peek == this->content.size()-1) break;
    def->content.push(consume());
  }
  if (notFound) Errors::error("Expected '#'", peek(-1)->line);
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
    int val = (tok->value.lit.u.i == 0) ? 1 : 0;
    return new Tokens::Token{Tokens::TokenType::LITERAL, tok->line, {.lit = {Literal::LiteralType::integer, {.i = val}}}};
  } else if (try_consume(Tokens::TokenType::DEFINED)) {
    if (peek()->type != Tokens::TokenType::IDENTIFIER) Errors::error("Expected Identifier", peek(-1)->line);
    Tokens::Token* ident = consume();
    int val = (definitions.contains(nominativeDef(std::string(ident->value.buffer)))) ? 1 : 0;
    return new Tokens::Token{Tokens::TokenType::LITERAL, ident->line, {.lit = {Literal::LiteralType::integer, {.i = val}}}};
  }
  return new Tokens::Token{Tokens::TokenType::NULL_TOKEN, -1};
}

void Preprocessor::Preprocessor::preprocess(Lists::List<Tokens::Token*>& ret, Lists::List<Definition*>& definitions) {
  if (try_consume(Tokens::TokenType::PREPROCESSOR)) {
    if (try_consume(Tokens::TokenType::DEFINE)) {
      if (peek()->type != Tokens::TokenType::IDENTIFIER) Errors::error("Expected Identifier", peek(-1)->line);
      Tokens::Token* ident = consume();
      if (definitions.contains(nominativeDef(std::string(ident->value.buffer))))
        Errors::error("Definition already exists", peek(-1)->line);
      definitions.push(preprocessDefine(ident));
    } else if (try_consume(Tokens::TokenType::UNDEFINE)) {
      if (peek()->type != Tokens::TokenType::IDENTIFIER) Errors::error("Expected Identifier", peek(-1)->line);
      Tokens::Token* ident = consume();
      if (!definitions.contains(nominativeDef(std::string(ident->value.buffer))))
        Errors::error("Definition does not exists", peek(-1)->line);
      definitions.pop(definitions.index(nominativeDef(std::string(ident->value.buffer))));
    } else if (try_consume(Tokens::TokenType::IF)) {
      if (!isComputable(definitions)) Errors::error("Expected Preprocessor Expression", peek(-1)->line);
      int val = compute(definitions)->value.lit.u.i;
      bool notFound = false;
      bool ignore = val == 0;
      if (!try_consume(Tokens::TokenType::OPEN_BRACKET)) Errors::error("Expected open bracket", peek(-1)->line);
      while ((notFound = !try_consume(Tokens::TokenType::CLOSE_BRACKET))) {
        if (_peek == this->content.size()-1) break;
        if (ignore)
          consume();
        else
          preprocess(ret, definitions);
      }
    } else if (try_consume(Tokens::TokenType::INCLUDE)) {
      if (peek()->type != Tokens::TokenType::LITERAL)
        Errors::error("Expected string literal path", peek(-1)->line);
      Tokens::Token* token = consume();
      if (token->value.lit.type != Literal::LiteralType::string)
        Errors::error("Expected string literal path", peek(-1)->line);
      
      char* path = token->value.lit.u.s;
      std::ifstream file{std::string(path)};
      std::stringstream content("");
      std::string buf;
      while (std::getline(file, buf)) {
        content << buf;
        content << "\n";
      };
      file.close();
      
      Tokens::Tokenizer inc{content};
      Lists::List<Tokens::Token*> tokens = inc.tokenize();
      Preprocessor pre{tokens};
      Lists::List<Tokens::Token*> cont = pre.preprocessAll();
      for (int i = 0; i < cont.size(); i++) {
        ret.push(cont.at(i));
      }
    }
  } else if (peek()->type == Tokens::TokenType::IDENTIFIER) {
    Tokens::Token* ident = consume();
    if (!definitions.contains(nominativeDef(std::string(ident->value.buffer)))) {
      ret.push(ident);
      return;
    }

    Definition* def = definitions.at(definitions.index(nominativeDef(std::string(ident->value.buffer))));
    Lists::List<Tokens::Token*> temp = def->content.copy();
    Dict::Dict<Tokens::Token*, Lists::List<Tokens::Token*>*> map{[](Tokens::Token* a, Tokens::Token* b){
        if (a->type != b->type) return false;
        if (a->type == Tokens::TokenType::IDENTIFIER) return a->value.buffer == b->value.buffer;
        if (a->type == Tokens::TokenType::LITERAL) {return a->value.lit == b->value.lit;}
        return true;
      }};
    if (peek()->type == Tokens::TokenType::SYMBOLS && std::string(consume()->value.buffer) == "<") {
      Lists::List<Tokens::Token*>* buf = new Lists::List<Tokens::Token*>{};
      int paramIndex = 0;
      while (!(peek()->type == Tokens::TokenType::SYMBOLS && std::string(consume()->value.buffer) == ">")) {
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
