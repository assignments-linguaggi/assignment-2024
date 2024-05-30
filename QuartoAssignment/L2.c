#include <stdio.h>

void foo(int c, int z) {
  int a = 9, h, m = 0, n = 0, q, r = 0, y = 0;

  // First loop
  while (z < 5) {
    z = z + 1;
    y = c + 3;
    q = c + 7;
    a = a + 2;
    h = c + 3;
    m = y + 7;
    n = h + 2;
    y = c + 7;
    r = q + 5;
  }

  // Second loop
  while (z < 10) {
    z = z + 1;
    y = c + 3;
    q = c + 7;
    a = a - 1;
    h = c + 4;
    m = y + 7;
    n = h + 2;
    y = c + 7;
    r = q + 5;
  }

  printf("%d,%d,%d,%d,%d,%d,%d,%d\n", a, h, m, n, q, r, y, z);
}

int main() {
  foo(0, 4);
  foo(0, 12);
  return 0;
}
