#include <stdio.h>

int foo() {
  printf("OK\n");
  return 42;
}

int foo2(int x, int y) {
  printf("OK, %d\n", x + y);
  return 11;
}

int foo6(int a, int b, int c, int d, int e, int f) {
  printf("OK, %d, %d, %d, %d, %d, %d\n", a, b, c, d, e, f);
  return 11;
}
