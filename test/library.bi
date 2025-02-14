type int membox 4 bits 32;
type long membox 8 bits 64;

cast<int a, long> asm {
  movsx rax, @a
};
