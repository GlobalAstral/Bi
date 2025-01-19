#include <iostream>
#include <Errors.hpp>

using std::string;
using std::cout;
using std::endl;
using std::exit;

namespace Errors {
  void error(string message) {
    cout << "An error has occurred: " << message << endl;
    exit(1);
  }

  void error(string message, int line) {
    cout << "An error has occurred at line " << line << ": " << message;
    exit(1);
  }
}
