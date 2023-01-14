#include <stdio.h>
#include <stdbool.h>
#include "9cc.h"

int main(int argc, char **argv) {
  if (argc != 2) {
    fprintf(stderr, "wrong number of arguments: expected 2, got %d\n", argc);
    return 1;
  }

  printf(".intel_syntax noprefix\n");
  printf(".globl _main\n");

  INPUT_HEAD = argv[1];
  CUR_TOKEN = tokenize(INPUT_HEAD);

  printf("_main:\n");
  Node *node = expr();
  print_node(node, true);
  gen(node);
  printf("  pop rax\n");
  printf("  ret\n");
  return 0;
}
