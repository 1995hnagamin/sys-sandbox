#ifndef IXCC_H
#define IXCC_H

typedef enum {
  TK_IF,
  TK_ELSE,
  TK_RETURN,
  TK_RESERVED,
  TK_IDENT,
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
  ND_INVALID,
  ND_IF,
  ND_RETURN,
  ND_ASSIGN, // assignments
  ND_ADD, // +
  ND_SUB, // -
  ND_MUL, // *
  ND_DIV, // /
  ND_EQU, // ==
  ND_NEQ, // !=
  ND_LES, // <
  ND_LEQ, // <=
  ND_INT, // integers
  ND_LVAR, // local variables
} NodeKind;

struct Node {
  NodeKind kind;
  struct Node *lhs;
  struct Node *rhs;
  int val; // valid if kind == ND_INT
  int offset; // valid if kind == ND_LVAR
};

typedef struct Node Node;

struct LVar {
  struct LVar *next;
  char *name;
  int len;
  int offset;
};
typedef struct LVar LVar;

extern Token *CUR_TOKEN;
extern char *INPUT_HEAD;

void error(char *fmt, ...);
void error_at(char *loc, char *fmt, ...);

Token *tokenize(char *p);

extern LVar *LOCAL_VARS;
extern Node *code[100];
void parse();

void view_node(Node *node);

void gen(Node *node);

#endif
