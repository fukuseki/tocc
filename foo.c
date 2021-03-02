#include <stdio.h>
#include <stdlib.h>

int foo() {
  printf("OK\n");
  return 42;
}

int foo2(int x, int y) {
  printf("OK, %d\n", x + y);
  return 11;
}

int foo4(int a, int b, int c, int d) {
  printf("OK, %d, %d, %d, %d\n", a, b, c, d);
  return a * b + c / d;
}

int foo6(int a, int b, int c, int d, int e, int f) {
  printf("OK, %d, %d, %d, %d, %d, %d\n", a, b, c, d, e, f);
  return 11;
}

void alloc4(int** p, int a, int b, int c, int d) {
  int* arr = calloc(4, sizeof(int));
  arr[0] = a;
  arr[1] = b;
  arr[2] = c;
  arr[3] = d;
  *p = arr;
}

void alloc2p(int*** p, int* a, int* b) {
  int** arr = calloc(2, sizeof(int*));
  arr[0] = a;
  arr[1] = b;
  *p = arr;
}
