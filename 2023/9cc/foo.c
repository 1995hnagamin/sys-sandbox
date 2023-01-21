#include <stdio.h>
#include <stdlib.h>

int foo() {
  printf("foo\n");
  return 42;
}

int bar(int a, int b) {
  printf("bar(%d,%d)\n", a, b);
  return 42;
}

int allocthree(int **p, int a, int b, int c) {
    int *arr = calloc(3, sizeof(int));
    printf("%p, %p, %p\n", (void*)arr, (void*)(arr+1), (void*)(arr+2));
    arr[0] = a;
    arr[1] = b;
    arr[2] = c;
    *p = arr;
    return 0;
}
