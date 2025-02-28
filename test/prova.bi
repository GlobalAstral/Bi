
type int membox 4 bits 32;

int a = 10;

asm {
  mov b32 @a, eax
}
