#pragma once

#include <iostream>

namespace Errors {
  void error(std::string message);
  void error(std::string message, int line);
}
