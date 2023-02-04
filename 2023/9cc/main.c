#include <errno.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "9cc.h"

char *read_file(char *path) {
  FILE *fp = fopen(path, "r");
  if (!fp) {
    error("cannot open %s: %s", path, strerror(errno));
  }

  if (fseek(fp, 0, SEEK_END) == -1) {
    error("%s: fseek: %s", path, strerror(errno));
  }
  size_t size = ftell(fp);
  if (fseek(fp, 0, SEEK_SET) == -1) {
    error("%s: fseek: %s", path, strerror(errno));
  }

  char *buf = calloc(1, size + 2);
  fread(buf, size, 1, fp);

  if (size == 0 || buf[size - 1] != '\n') {
    buf[size++] = '\n';
  }
  buf[size] = '\0';
  fclose(fp);
  return buf;
}

int main(int argc, char **argv) {
  if (argc != 2) {
    fprintf(stderr, "wrong number of arguments: expected 2, got %d\n", argc);
    return 1;
  }


  INPUT_HEAD = read_file(argv[1]);
  CUR_TOKEN = tokenize(INPUT_HEAD);
  parse();

  printf(".intel_syntax noprefix\n");
  gen_str_lit();
  printf(".globl _main\n");
  for (int i = 0; code[i]; ++i) {
    view_node(code[i]);
    fprintf(stderr, "\n");
    gen(code[i]);
  }
  return 0;
}
