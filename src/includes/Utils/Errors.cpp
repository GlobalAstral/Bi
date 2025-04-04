#include <Utils/Errors.hpp>

void Errors::error(std::string type, std::string error, int line) {
  std::cout << RED << "ERROR" << ((line > 0) ? (" AT LINE " + line) : "") << ':' << type << " -> " << error << RESET << "\n";
  exit(1);
}

void Errors::warn(std::string warning) {
  std::cout << YELLOW << "WARNING: " << warning << RESET << "\n";
}

void Errors::info(std::string i) {
  std::cout << BLUE << "INFO: " << i << RESET << "\n";
}

#undef RED
#undef YELLOW
#undef BLUE
#undef RESET
