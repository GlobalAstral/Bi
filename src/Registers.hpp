
#pragma once

#include <iostream>
#include <Errors.hpp>

namespace Registers {
  enum class RegisterType {
    b8, b16, b32, b64, simd, invalid
  };
  struct RegMapping {
    const char* A;
    const char* B;
    const char* C;
    const char* D;
    const char* SI;
    const char* DI;
    const char* BP;
    const char* SP;
    const char* R8;
    const char* R9;
    const char* R10;
    const char* R11;
    const char* R12;
    const char* R13;
    const char* R14;
    const char* R15;
  };

  RegMapping getMappings(RegisterType type);
  RegisterType getRegType(char*);
}

