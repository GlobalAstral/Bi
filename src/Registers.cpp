#include <Registers.hpp>

Registers::RegMapping Registers::getMappings(Registers::RegisterType type) {
  switch (type) {
    case RegisterType::b8 :
      return {"al", "bl", "cl", "dl", "sil", "dil", "bpl", "spl", "r8b", "r9b", "r10b", "r11b", "r12b", "r13b", "r14b", "r15b"};
    case RegisterType::b16 :
      return {"ax", "bx", "cx", "dx", "si", "di", "bp", "sp", "r8w", "r9w", "r10w", "r11w", "r12w", "r13w", "r14w", "r15w"};
    case RegisterType::b32 :
      return {"eax", "ebx", "ecx", "edx", "esi", "edi", "ebp", "esp", "r8d", "r9d", "r10d", "r11d", "r12d", "r13d", "r14d", "r15d"};
    case RegisterType::b64 :
      return {"rax", "rbx", "rcx", "rdx", "rsi", "rdi", "rbp", "rsp", "r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15"};
    case RegisterType::simd :
      return {"xmm0", "xmm1", "xmm2", "xmm3", "xmm4", "xmm5", "xmm6", "xmm7", "xmm8", "xmm9", "xmm10", "xmm11", "xmm12", "xmm13", "xmm14", "xmm15"};
  }
  return {};
}

Registers::RegisterType Registers::getRegType(char* str) {
  std::string s = std::string(str);
  if (s == "b8")
    return RegisterType::b8;
  if (s == "b16")
    return RegisterType::b16;
  if (s == "b32")
    return RegisterType::b32;
  if (s == "b64")
    return RegisterType::b64;
  return RegisterType::invalid;
}
