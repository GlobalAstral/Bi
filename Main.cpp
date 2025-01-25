#include <iostream>
#include <sstream>
#include <fstream>

#include <Errors.hpp>
#include <string.h>
#include <Tokenizer.hpp>
#include <Preprocessor.hpp>
#include <Parser.hpp>

using std::cout;
using std::endl;
using std::string;

//TODO SCATOLE DI MEMORIA

string replace(string input, string src, string dest) {
  size_t start = input.find(src);
  if (start == string::npos)
    Errors::error("Cannot find string '" + src + "'");
  string output = input.replace(start, src.length(), dest);
  return output;
}

bool commandLineOption(string option, char** argv, int argc) {
  for (int i = 0; i < argc; i++) {
    if (strcmp(argv[i], option.c_str()) == 0)
      return true;
  }
  return false;
}

int main(int argc, char** argv) {
  const string EXTENSION = ".bi";
  if (argc <= 1)
    Errors::error("Not enough command line arguments");

  string bi_filepath = argv[1];

  string asm_filepath = replace(bi_filepath, EXTENSION, ".asm");
  string obj_filepath = replace(bi_filepath, EXTENSION, ".obj");
  string exe_filepath = replace(bi_filepath, EXTENSION, ".exe");
  if (argc > 2 && argv[2][0] != '-') exe_filepath = argv[2];

  bool asmCode = commandLineOption("-asm", argv, argc);
  bool objCode = commandLineOption("-obj", argv, argc);

  std::ifstream bi_file(bi_filepath);
  std::stringstream content("");
  string buf;
  while (std::getline(bi_file, buf)) {
    content << buf;
    content << "\n";
  };
  bi_file.close();

  Tokens::Tokenizer tokenizer(content);
  Lists::List<Tokens::Token*> tokens = tokenizer.tokenize();
  cout << endl << "TOKENS:" << endl;
  for (int i = 0; i < tokens.size(); i++)
    cout << tokens.at(i)->toString() << endl;

  Preprocessor::Preprocessor preprocessor(tokens);
  Lists::List<Tokens::Token*> preprocessed = preprocessor.preprocessAll();
  cout << endl << "PREPROCESSED TOKENS:" << endl;
  for (int i = 0; i < preprocessed.size(); i++)
    cout << preprocessed.at(i)->toString() << endl;

  Parser::Parser parser{preprocessed};
  Lists::List<Nodes::Statement*> stmts = parser.parseStmts();
  cout << endl << "STATEMENTS:" << endl;
  for (int i = 0; i < stmts.size(); i++)
    cout << stmts.at(i)->toString() << endl;

	return 0;
} 
