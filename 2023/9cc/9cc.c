#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

Token *new_token(TokenKind kind, Token *cur, char *str) {
  Token *tok = calloc(1, sizeof(Token));
  tok->kind = kind;
  tok->str = str;
  cur->next = tok;
  return tok;
}

Token *CUR_TOKEN;
char *INPUT_HEAD;

void error_at(char *loc, char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);

  int pos = loc - INPUT_HEAD;
  fprintf(stderr, "%s\n", INPUT_HEAD);
  for (int i = 0; i < pos; ++i) {
    fprintf(stderr, " ");
  }
  fprintf(stderr, "^ ");
  fprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

bool is_operator(char *op, Token *token) {
  return token->kind == TK_RESERVED
    && ((int)strlen(op)) == token->len
    && memcmp(token->str, op, token->len) == 0;
}

bool consume(char *op) {
  if (!is_operator(op, CUR_TOKEN)) {
    return false;
  }
  CUR_TOKEN = CUR_TOKEN->next;
  return true;
}

void expect(char *op) {
  if (!is_operator(op, CUR_TOKEN)) {
    error_at(CUR_TOKEN->str, "not equal to '%c'", op);
  }
  CUR_TOKEN = CUR_TOKEN->next;
}

int expect_number() {
  if (CUR_TOKEN->kind != TK_NUM) {
    error_at(CUR_TOKEN->str, "not a number");
  }
  int val = CUR_TOKEN->val;
  CUR_TOKEN = CUR_TOKEN->next;
  return val;
}

bool at_eof() {
  return CUR_TOKEN->kind == TK_EOF;
}

bool is_2char_symbol(char *p) {
  return strncmp(p, "<=", 2) == 0 || strncmp(p, ">=", 2) == 0;
}

bool is_symbol(char c) {
  return c == '+' || c == '-'
    || c == '*' || c == '/'
    || c == '<' || c == '>'
    || c == '(' || c == ')';
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
    if (isdigit(*p)) {
      cur = new_token(TK_NUM, cur, p);
      cur->val = strtol(p, &p, 10);
      continue;
    }
    error_at(p, "tokenization failed");
  }
  new_token(TK_EOF, cur, p);
  return head.next;
}

typedef enum {
  ND_ADD, // +
  ND_SUB, // -
  ND_MUL, // *
  ND_DIV, // /
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

Node *new_node(NodeKind kind, Node *lhs, Node *rhs) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = kind;
  node->lhs = lhs;
  node->rhs = rhs;
  return node;
}

void print_node(Node *node, bool nl) {
  if (node->kind == ND_INT) {
    fprintf(stderr, "%d", node->val);
    if (nl) {
      fprintf(stderr, "\n");
    }
    return;
  }
  fprintf(stderr, "(");
  switch (node->kind) {
  case ND_ADD:
    fprintf(stderr, "+");
    break;
  case ND_SUB:
    fprintf(stderr, "-");
    break;
  case ND_MUL:
    fprintf(stderr, "*");
    break;
  case ND_DIV:
    fprintf(stderr, "/");
    break;
  case ND_LES:
    fprintf(stderr, "<");
    break;
  case ND_LEQ:
    fprintf(stderr, "<=");
    break;
  default:
    fprintf(stderr, "fatal error");
    exit(1);
  }
  fprintf(stderr, " ");
  print_node(node->lhs, false);
  fprintf(stderr, " ");
  print_node(node->rhs, false);
  fprintf(stderr, ")");
  if (nl) {
    fprintf(stderr, "\n");
  }
}

Node *new_node_num(int val) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = ND_INT;
  node->val = val;
  return node;
}

Node *add();
Node *rational();
Node *mul();
Node *unary();
Node *primary();

Node *expr() {
  return rational();
}

Node *rational() {
  Node *node = add();
  for (;;) {
    if (consume("<=")) {
      node = new_node(ND_LEQ, node, add());
    } else if (consume("<")) {
      node = new_node(ND_LES, node, add());
    } else if (consume(">=")) {
      node = new_node(ND_LES, add(), node);
    } else if (consume(">")) {
      node = new_node(ND_LEQ, add(), node);
    } else {
      return node;
    }
  }
}

Node *add() {
  Node *node = mul();
  for (;;) {
    if (consume("+")) {
      node = new_node(ND_ADD, node, mul());
    } else if (consume("-")) {
      node = new_node(ND_SUB, node, mul());
    } else {
      return node;
    }
  }
}

Node *mul() {
  Node *node = unary();

  for (;;) {
    if (consume("*")) {
      node = new_node(ND_MUL, node, unary());
    } else if (consume("/")) {
      node = new_node(ND_DIV, node, unary());
    } else {
      return node;
    }
  }
}

Node *unary() {
  if (consume("+")) {
    return primary();
  } else if (consume("-")) {
    return new_node(ND_SUB, new_node_num(0), primary());
  }
  return primary();
}

Node *primary() {
  if (consume("(")) {
    Node *node = expr();
    expect(")");
    return node;
  }
  return new_node_num(expect_number());
}

void gen(Node *node) {
  if (node->kind == ND_INT) {
    printf("  push %d\n", node->val);
    return;
  }
  gen(node->lhs);
  gen(node->rhs);

  printf("  pop rdi\n");
  printf("  pop rax\n");
  switch (node->kind) {
  case ND_ADD:
    printf("  add rax, rdi\n");
    break;
  case ND_SUB:
    printf("  sub rax, rdi\n");
    break;
  case ND_MUL:
    printf("  imul rax, rdi\n");
    break;
  case ND_DIV:
    printf("  cqo\n");
    printf("  idiv rdi\n");
    break;
  case ND_LES:
    printf("  cmp rax, rdi\n");
    printf("  setl al\n");
    printf("  movzx rax, al\n");
    break;
  case ND_LEQ:
    printf("  cmp rax, rdi\n");
    printf("  setle al\n");
    printf("  movzx rax, al\n");
    break;
  case ND_INT:
    fprintf(stderr, "fatal error");
    exit(1);
  }
  printf("  push rax\n");
}

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
