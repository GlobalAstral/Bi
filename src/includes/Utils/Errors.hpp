#pragma once
#include <iostream>

#define RED "\033[31m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define RESET "\033[0m"

namespace Errors {
  [[noreturn]] void error(std::string type, std::string error, int line = -1);
  void warn(std::string warning);
  void info(std::string i);
}


