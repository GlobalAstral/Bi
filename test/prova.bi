type int membox 4 bits 32;

operation<int a, int b, "+"> asm {
  mov eax, @a
  add @a, @b
} int;
