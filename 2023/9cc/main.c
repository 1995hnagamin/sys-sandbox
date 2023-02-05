#include <errno.h>
#include <stdarg.h>
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


  FILENAME = argv[1];
  INPUT_HEAD = read_file(FILENAME);
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

void error(char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);

  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

char *INPUT_HEAD;
char *FILENAME;

void error_at(char *loc, char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  char *line = loc;
  while (INPUT_HEAD < line && line[-1] != '\n') {
    line--;
  }

  char *end = loc;
  while (*end != '\n') {
    end++;
  }

  int line_num = 1;
  for (char *p = INPUT_HEAD; p < line; p++)
    if (*p == '\n') {
      line_num++;
    }

  int indent = fprintf(stderr, "%s:%d: ", FILENAME, line_num);
  fprintf(stderr, "%.*s\n", (int)(end - line), line);

  int pos = loc - line + indent;
  for (int i = 0; i < pos; ++i) {
    fprintf(stderr, " ");
  }
  fprintf(stderr, "^ ");
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}
