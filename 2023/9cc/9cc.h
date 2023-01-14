#ifndef IXCC_H
#define IXCC_H

typedef enum {
  TK_RESERVED,
  TK_NUM,
  TK_EOF,
} TokenKind;

struct Token {
  TokenKind kind;
  struct Token *next;
  int val;
  char *str;
  int len;
};

typedef struct Token Token;

typedef enum {
  ND_ADD, // +
  ND_SUB, // -
  ND_MUL, // *
  ND_DIV, // /
  ND_EQU, // ==
  ND_NEQ, // !=
  ND_LES, // <
  ND_LEQ, // <=
  ND_INT, // integers
} NodeKind;

struct Node {
  NodeKind kind;
  struct Node *lhs;
  struct Node *rhs;
  int val;
};

typedef struct Node Node;

extern Token *CUR_TOKEN;
extern char *INPUT_HEAD;

void error(char *fmt, ...);
void error_at(char *loc, char *fmt, ...);

Token *tokenize(char *p);

Node *expr();

void print_node(Node *node, bool nl);

void gen(Node *node);

#endif
