#include <Tokenizer/Preprocessor.hpp>
#include "Preprocessor.hpp"

Preprocessor::Preprocessor::Preprocessor(std::vector<Tokens::Token>& toks) {
  this->tokens = toks;
}

std::vector<Tokens::Token> Preprocessor::Preprocessor::preprocess() {
  using namespace std;
  vector<Tokens::Token> ret{};

  while (hasPeek()) {
    if (tryconsume(Tokens::TokenType::preprocessor)) {

      if (tryconsume(Tokens::TokenType::define)) {
        Tokens::Token ident = tryconsume(Tokens::TokenType::identifier, {"Missing Token", "Expected Identifier"});
        std::vector<string> params;
        if (tryconsume(Tokens::TokenType::open_paren)) {
          bool notFound = true;
          while (notFound = hasPeek()) {
            Tokens::Token tok = tryconsume(Tokens::TokenType::identifier, {"Missing Token", "Expected Identifier"});
            params.push_back(tok.value);
            if (tryconsume(Tokens::TokenType::close_paren)) {
              notFound = false;
              break;
            }
            tryconsume(Tokens::TokenType::comma, {"Missing Token", "Expected comma"});
          }
          if (notFound)
            error({"Missing token", "Closing curly bracket expected"});
        }
        std::vector<Tokens::Token> content;
        bool notFound = true;
        while (hasPeek()) {
          Tokens::Token tok = consume();
          content.push_back(tok);
          if (tryconsume(Tokens::TokenType::preprocessor)) {
            notFound = false;
            break;
          }
        }
        if (notFound)
          error({"Missing token", "Closing preprocessor expected"});
        definitions[ident.value] = *(new Definition{params, content});
      } else if (tryconsume(Tokens::TokenType::undefine)) {
        Tokens::Token ident = tryconsume(Tokens::TokenType::identifier, {"Missing Token", "Expected Identifier"});
        if (!definitions.contains(ident.value))
          error({"Definition not found", Formatting::format("The definition %s does not exist", ident.value.c_str())});
        definitions.remove(ident.value);
      }

    } else if (peek().type == Tokens::TokenType::identifier) {
      preprocessIdentifier(consume(), ret);
    } else {
      ret.push_back(consume());
    }
  }

  return ret;
}

int Preprocessor::Preprocessor::preprocessIdentifier(Tokens::Token ident, std::vector<Tokens::Token>& out) {
  int start = _peek;
  if (!definitions.contains(ident.value)) {
    out.push_back(ident);
    return 0;
  }
  Definition def = definitions[ident.value];
  std::vector<std::vector<Tokens::Token>> params;

  if (tryconsume(Tokens::TokenType::open_paren)) {
    bool notFound = true;
    while (hasPeek()) {
      std::vector<Tokens::Token> param;
      while (peek().type != Tokens::TokenType::comma && peek().type != Tokens::TokenType::close_paren) {
        param.push_back(consume());
      }
      params.push_back(param);
      if (tryconsume(Tokens::TokenType::close_paren)) {
        notFound = false;
        break;
      }
      tryconsume(Tokens::TokenType::comma, {"Missing Token", "Expected comma"});
    }
    if (notFound)
      error({"Missing token", "Closing curly bracket expected"});
  }
  
  if (def.params.size() != params.size())
    error({"Invalid definition parameters", "No definition found with such parameters"});
  
  for (int i = 0; i < def.content.size(); i++) {
    Tokens::Token token = def.content[i];
    int paramIndex = VectorUtils::find(def.params, token.value);
    if (token.type != Tokens::TokenType::identifier || (paramIndex < 0 && !definitions.contains(token.value))) {
      out.push_back(token);
      continue;
    }
    if (paramIndex > -1) {
      std::vector<Tokens::Token> param = params[paramIndex];
      for (auto tok : param) {
        out.push_back(tok);
      }
      continue;
    }
    std::vector<Tokens::Token> prev = tokens;
    int prev_peek = _peek;

    _peek = 1;
    tokens = def.content;

    i = i + preprocessIdentifier(token, out);

    _peek = prev_peek;
    tokens = prev;
  }
  int end = _peek;
  return end - start;
}

bool Preprocessor::Preprocessor::hasPeek(int offset) {
  return _peek+offset >= 0 && _peek+offset < this->tokens.size();
}

Tokens::Token Preprocessor::Preprocessor::peek(int offset) {
  return (hasPeek(offset)) ? this->tokens[_peek+offset] : Tokens::nullToken();
}

Tokens::Token Preprocessor::Preprocessor::consume() {
  return (hasPeek()) ? this->tokens[_peek++] : Tokens::nullToken();
}

bool Preprocessor::Preprocessor::tryconsume(Tokens::TokenType type) {
  if (peek().type == type) {
    consume();
    return true;
  }
  return false;
}

Tokens::Token Preprocessor::Preprocessor::tryconsume(Tokens::TokenType type, Errors::CompactError error) {
  if (peek().type == type)
    return consume();
  this->error(error);
}

void Preprocessor::Preprocessor::error(Errors::CompactError error) {
  Errors::error(error, peek(-1).line);
}
