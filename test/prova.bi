
unary !: <uint a, uint, 1> {
  if (a)
    return 0;
  return 1;
}

unary -: <uint a, uint, 2> {
  asm {
    mov eax, 0
    mov ecx, %a%
    sub eax, ecx
  };
}

binary +: <uint a, uint b, uint, 1> {
  asm {
    mov eax, %a%
    mov ecx, %b%
    add eax, ecx
  };
}

binary -: <uint a, uint b, uint, 1> {
  return a + (-b);
}

binary ==: <uint a, uint b, uint, 1> {
  return !(a - b);
}

binary ||:<uint a, uint b, uint, 1> {
  if (a)
    return 1;
  if (b)
    return 1;
  return 0;
}

method uint fibonacci(uint n) {
  if (n == 0 || n == 1)
    return 1;
  return fibonacci(n-1)+fibonacci(n-2);
}
