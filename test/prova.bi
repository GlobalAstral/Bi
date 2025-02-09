type int membox 4 bits 32;

operation<int a, int b, +, 2> asm {
  mov eax, @a
  add @a, @b
} int;

operation<int a, int b, *, 3> asm {
  mov eax, @a
  mov ecx, @b
  mul eax
} int;

int a = 69 + 420 * 1;
