#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "9cc.h"


Token *CUR_TOKEN;

static Token *new_token(TokenKind kind, Token *cur, char *str) {
  Token *tok = calloc(1, sizeof(Token));
  tok->kind = kind;
  tok->str = str;
  cur->next = tok;
  return tok;
}

static int is_alnum(char c) {
  return ('a' <= c && c <= 'z')
    || ('A' <= c && c <= 'Z')
    || ('0' <= c && c <= '9')
    || (c == '_');
}

static bool at_eof() {
  return CUR_TOKEN->kind == TK_EOF;
}

static bool is_2char_symbol(char *p) {
  return strncmp(p, "<=", 2) == 0 || strncmp(p, ">=", 2) == 0
    || strncmp(p, "==", 2) == 0 || strncmp(p, "!=", 2) == 0;
}

static bool is_symbol(char c) {
  return c == '+' || c == '-'
    || c == '*' || c == '/'
    || c == '&'
    || c == '<' || c == '>'
    || c == '(' || c == ')'
    || c == '[' || c == ']'
    || c == '{' || c == '}'
    || c == '=' || c == ','
    || c == ';';
}

Token *tokenize(char *p) {
  Token head;
  head.next = NULL;
  Token *cur = &head;
  while (*p) {
    if (isspace(*p)) {
      p++;
      continue;
    }
    if (strncmp("char", p, 4) == 0 && !is_alnum(p[4])) {
      cur = new_token(TK_CHAR, cur, p);
      p += 4;
      continue;
    }
    if (strncmp("if", p, 2) == 0 && !is_alnum(p[2])) {
      cur = new_token(TK_IF, cur, p);
      p += 2;
      continue;
    }
    if (strncmp("int", p, 2) == 0 && !is_alnum(p[3])) {
      cur = new_token(TK_INT, cur, p);
      p += 3;
      continue;
    }
    if (strncmp("else", p, 4) == 0 && !is_alnum(p[4])) {
      cur = new_token(TK_ELSE, cur, p);
      p += 4;
      continue;
    }
    // if p starts with "return", p[6] should be exist
    if (strncmp("return", p, 6) == 0 && !is_alnum(p[6])) {
      cur = new_token(TK_RETURN, cur, p);
      p += 6;
      continue;
    }
    if (strncmp("sizeof", p, 6) == 0 && !is_alnum(p[6])) {
      cur = new_token(TK_SIZEOF, cur, p);
      p += 6;
      continue;
    }
    if (strncmp("while", p, 5) == 0 && !is_alnum(p[5])) {
      cur = new_token(TK_WHILE, cur, p);
      p += 5;
      continue;
    }
    if (is_2char_symbol(p)) {
      cur = new_token(TK_RESERVED, cur, p);
      cur->len = 2;
      p += 2;
      continue;;
    }
    if (is_symbol(*p)) {
      cur = new_token(TK_RESERVED, cur, p++);
      cur->len = 1;
      continue;
    }
    if ('a' <= *p && *p <= 'z') {
        cur = new_token(TK_IDENT, cur, p);
        char *start = p++;
        while ('a' <= *p && *p <= 'z') { ++p; }
        cur->len = p - start;
        continue;
    }
    if (isdigit(*p)) {
      cur = new_token(TK_NUM, cur, p);
      cur->val = strtol(p, &p, 10);
      continue;
    }
    if (*p == '"') {
      char *start = ++p;
      cur = new_token(TK_STR, cur, start);
      while (*p != '"') { ++p; }
      cur->len = (p++) - start;
      continue;
    }
    error_at(p, "tokenization failed");
  }
  new_token(TK_EOF, cur, p);
  return head.next;
}
