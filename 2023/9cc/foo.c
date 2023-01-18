#include <stdio.h>

int foo() {
  printf("foo\n");
  return 42;
}

int bar(int a, int b) {
  printf("bar(%d,%d)\n", a, b);
  return 42;
}
