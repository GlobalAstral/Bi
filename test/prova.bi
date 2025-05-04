
unary - : <uint a, int, 1> asm {
  mov eax, %a%
  mov ecx, -1
  mul eax, ecx
}
