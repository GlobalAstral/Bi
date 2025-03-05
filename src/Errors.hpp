#pragma once

#include <iostream>

namespace Errors {
  [[noreturn]] void error(std::string message);
  [[noreturn]] void error(std::string message, int line);
}
