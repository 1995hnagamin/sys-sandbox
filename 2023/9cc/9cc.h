#ifndef IXCC_H
#define IXCC_H

typedef enum {
  TK_INVALID = 0,
  TK_CHAR,
  TK_IF,
  TK_INT,
  TK_ELSE,
  TK_RETURN,
  TK_SIZEOF,
  TK_RESERVED,
  TK_IDENT,
  TK_NUM,
  TK_STR,
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
  TY_INVALID = 0,
  TY_CHAR,
  TY_INT,
  TY_ARRAY,
  TY_FN,
  TY_PTR,
} TypeKind;

struct Type {
  TypeKind kind;
  struct Type *ptr_to;
  size_t array_size; // valid if kind == TY_ARRAY
  struct Node *params;
};
typedef struct Type Type;

Type *ty_reserved(TypeKind tyk);
Type *new_ty_array(size_t n, Type *to);
Type *new_ty_fn(struct Node *params, Type *ret);
Type *new_ty_ptr(Type *to);

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
  ND_STR,
  ND_LVAR, // local variables
  ND_GVAR, // global variables
  ND_BLOCK, // compound statements
} NodeKind;

struct Node {
  NodeKind kind;
  struct Node *lhs;
  struct Node *rhs;
  int val; // valid if kind == ND_INT
  struct LVar *lvar; // valid if kind == ND_LVAR
  struct GVar *gvar;
  Token *tok; // valid if kind == ND_FNCALL
  struct Type *ty;
};

typedef struct Node Node;

void set_type(Node *node);

struct LVar {
  struct LVar *next;
  char *name;
  int len;
  int offset;
  Type *ty;
};
typedef struct LVar LVar;

struct GVar {
  struct GVar *next;
  char *name;
  int len;
  Type *ty;
};
typedef struct GVar GVar;

extern Token *CUR_TOKEN;
extern char *INPUT_HEAD;

struct StrLit {
  struct StrLit *next;
  char *str;
  int len;
  int idx;
};
typedef struct StrLit StrLit;
extern StrLit *STR_LIT;

void error(char *fmt, ...);
void error_at(char *loc, char *fmt, ...);

Token *tokenize(char *p);

extern LVar *LOCAL_VARS;
extern Node *code[100];
void parse();

void view_token(Token *tok);
void view_node(Node *node);

int nbytes_type(Type *ty);
void gen_str_lit();
void gen(Node *node);

#endif
