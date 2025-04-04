#include <Utils/StringUtils.hpp>

bool StringUtils::isInString(char c, std::string s) {
  for (char ch : s) {
    if (c == ch)
      return true;
  }
  return false;
}