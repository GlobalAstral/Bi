#include <Tokenizer/Preprocessor.hpp>
#include "Preprocessor.hpp"

Preprocessor::Preprocessor::Preprocessor(std::vector<Tokens::Token>& toks) {
  this->content = toks;
}

std::vector<Tokens::Token> Preprocessor::Preprocessor::preprocess() {
  using namespace std;
  vector<Tokens::Token> ret{};

  while (hasPeek()) {
    preprocessSingle(ret);
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

  if (tryconsume({Tokens::TokenType::open_paren})) {
    bool notFound = true;
    while (hasPeek()) {
      std::vector<Tokens::Token> param;
      while (peek().type != Tokens::TokenType::comma && peek().type != Tokens::TokenType::close_paren) {
        param.push_back(consume());
      }
      params.push_back(param);
      if (tryconsume({Tokens::TokenType::close_paren})) {
        notFound = false;
        break;
      }
      tryconsume({Tokens::TokenType::comma}, {"Missing Token", "Expected comma"});
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
    std::vector<Tokens::Token> prev = content;
    int prev_peek = _peek;

    _peek = 1;
    content = def.content;

    i = i + preprocessIdentifier(token, out);

    _peek = prev_peek;
    content = prev;
  }
  int end = _peek;
  return end - start;
}

bool Preprocessor::Preprocessor::preprocessBoolean() {
  bool inverted = peek().type == Tokens::TokenType::symbols && consume().value == "!";
  bool flag = false;
  if (tryconsume({Tokens::TokenType::defined})) {
    Tokens::Token ident = tryconsume({Tokens::TokenType::identifier}, {"Missing Token", "Expected Identifier"});
    flag = definitions.contains(ident.value);
  }
  return inverted ? !flag : flag;
}

void Preprocessor::Preprocessor::preprocessSingle(std::vector<Tokens::Token>& ret) {
  using namespace std;
  Tokens::Token temp = peek();
  if (tryconsume({Tokens::TokenType::preprocessor})) {

    if (tryconsume({Tokens::TokenType::define})) {
      Tokens::Token ident = tryconsume({Tokens::TokenType::identifier}, {"Missing Token", "Expected Identifier"});
      std::vector<string> params;
      if (tryconsume({Tokens::TokenType::open_paren})) {
        bool notFound = true;
        while (hasPeek()) {
          Tokens::Token tok = tryconsume({Tokens::TokenType::identifier}, {"Missing Token", "Expected Identifier"});
          params.push_back(tok.value);
          if (tryconsume({Tokens::TokenType::close_paren})) {
            notFound = false;
            break;
          }
          tryconsume({Tokens::TokenType::comma}, {"Missing Token", "Expected comma"});
        }
        if (notFound)
          error({"Missing token", "Closing curly bracket expected"});
      }
      std::vector<Tokens::Token> content;
      bool notFound = true;
      while (hasPeek()) {
        Tokens::Token tok = consume();
        content.push_back(tok);
        if (tryconsume({Tokens::TokenType::preprocessor})) {
          notFound = false;
          break;
        }
      }
      if (notFound)
        error({"Missing token", "Closing preprocessor expected"});
      definitions[ident.value] = *(new Definition{params, content});
    } else if (tryconsume({Tokens::TokenType::undefine})) {

      Tokens::Token ident = tryconsume({Tokens::TokenType::identifier}, {"Missing Token", "Expected Identifier"});
      if (!definitions.contains(ident.value))
        error({"Definition not found", Formatting::format("The definition %s does not exist", ident.value.c_str())});
      definitions.remove(ident.value);
      tryconsume({Tokens::TokenType::preprocessor}, {"Missing Token", "Expected closing preprocessor"});

    } else if (tryconsume({Tokens::TokenType::If})) {
      bool flag = preprocessBoolean();
      
      bool isElse = false;

      while (hasPeek()) {
        if (peek().type == Tokens::TokenType::preprocessor) {
          if (peek(1).type == Tokens::TokenType::endif) {
            consume(2);
            break;
          }
        }
        if (tryconsume({Tokens::TokenType::Else})) {
          isElse = true;
          continue;
        }
        if (tryconsume({Tokens::TokenType::elseif})) {
          flag = !flag && preprocessBoolean();
          continue;
        }
        if ((!isElse && flag) || (isElse && !flag)) {
          preprocessSingle(ret);
          continue;
        }
        consume();
      }
      
    } else if (tryconsume({Tokens::TokenType::include})) {
      using std::string, std::istream, std::stringstream;
      Tokens::Token token = tryconsume({Tokens::TokenType::literal}, {"Missing Token", "Expected string literal"});
      string filename = token.value;
      if (filename[0] != '"' || filename[filename.size()-1] != '"')
        error({"Invalid File", "Expected string literal for file name"});
      filename.erase(0, 1);
      filename.erase(filename.size()-1, 1);
      filename.append(".bi");
      if (!filesystem::is_regular_file(filename)) 
        error({"Invalid File", Formatting::format("Cannot include file '%s'", token.value.c_str())});
      
      stringstream buf;
      string temp;
      
      ifstream fstr{filename};
      while (std::getline(fstr, temp))
        buf << temp;
      fstr.close();

      Tokenizer::Tokenizer tokenizer{buf.str()};
      vector<Tokens::Token> tokens = tokenizer.tokenize();
      Preprocessor preprocessor{tokens};
      tokens = preprocessor.preprocess();

      for (auto token : tokens)
        ret.push_back(token);
    }

  } else if (peek().type == Tokens::TokenType::identifier) {
    preprocessIdentifier(consume(), ret);

  } else {
    ret.push_back(consume());
  }
}

Tokens::Token Preprocessor::Preprocessor::null() {
  return Tokens::nullToken();
}

int Preprocessor::Preprocessor::getCurrentLine() {
  return peek(-1).line;
}

bool Preprocessor::Preprocessor::equalCriteria(Tokens::Token a, Tokens::Token b) {
  return a.type == b.type;
}
