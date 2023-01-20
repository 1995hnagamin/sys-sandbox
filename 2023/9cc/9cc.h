#ifndef IXCC_H
#define IXCC_H

typedef enum {
  TK_IF,
  TK_INT,
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

struct Type {
  enum {
    TY_INT,
    TY_PTR,
  } ty;
  struct Type *ptr_to;
};
typedef struct Type Type;

typedef enum {
  ND_INVALID,
  ND_FNDEF,
  ND_DECL,
  ND_IF,
  ND_RETURN,
  ND_ASSIGN, // assignments
  ND_FNCALL, // function call
  ND_ADD, // +
  ND_SUB, // -
  ND_MUL, // *
  ND_DIV, // /
  ND_EQU, // ==
  ND_NEQ, // !=
  ND_LES, // <
  ND_LEQ, // <=
  ND_ADDR, // &
  ND_DEREF, // *
  ND_INT, // integers
  ND_LVAR, // local variables
  ND_BLOCK, // compound statements
} NodeKind;

struct Node {
  NodeKind kind;
  struct Node *lhs;
  struct Node *rhs;
  int val; // valid if kind == ND_INT
  struct LVar *lvar; // valid if kind == ND_LVAR
  Token *tok; // valid if kind == ND_FNCALL
  struct Type *ty;
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
