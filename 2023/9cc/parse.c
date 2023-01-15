#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "9cc.h"

Token *CUR_TOKEN;
char *INPUT_HEAD;

Token *new_token(TokenKind kind, Token *cur, char *str) {
  Token *tok = calloc(1, sizeof(Token));
  tok->kind = kind;
  tok->str = str;
  cur->next = tok;
  return tok;
}

void error(char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);

  fprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

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

int is_alnum(char c) {
  return ('a' <= c && c <= 'z')
    || ('A' <= c && c <= 'Z')
    || ('0' <= c && c <= '9')
    || (c == '_');
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

Token *consume_ident() {
  Token *ident = CUR_TOKEN;
  if (ident->kind != TK_IDENT) {
    return NULL;
  }
  CUR_TOKEN = CUR_TOKEN->next;
  return ident;
}

bool consume_tk(TokenKind tk) {
  if (CUR_TOKEN->kind != tk) {
    return false;
  }
  CUR_TOKEN = CUR_TOKEN->next;
  return true;
}

void expect(char *op) {
  if (!is_operator(op, CUR_TOKEN)) {
    error_at(CUR_TOKEN->str, "not equal to \"%s\"", op);
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
  return strncmp(p, "<=", 2) == 0 || strncmp(p, ">=", 2) == 0
    || strncmp(p, "==", 2) == 0 || strncmp(p, "!=", 2) == 0;
}

bool is_symbol(char c) {
  return c == '+' || c == '-'
    || c == '*' || c == '/'
    || c == '<' || c == '>'
    || c == '(' || c == ')'
    || c == '{' || c == '}'
    || c == '='
    || c == ';';
}

LVar *LOCAL_VARS;
LVar *find_lvar(Token *tok) {
  int len = tok->len;
  for (LVar *var = LOCAL_VARS; var; var = var->next) {
    if (var->len == len && !memcmp(tok->str, var->name, len)) {
      return var;
    }
  }
  return NULL;
}

LVar *new_lvar(struct LVar *next, char *name, int len, int offset) {
  LVar *lvar = calloc(1, sizeof(LVar));
  lvar->next = next;
  lvar->name = name;
  lvar->len = len;
  lvar->offset = offset;
  return lvar;
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
    if (strncmp("if", p, 2) == 0 && !is_alnum(p[2])) {
      cur = new_token(TK_IF, cur, p);
      p += 2;
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
    error_at(p, "tokenization failed");
  }
  new_token(TK_EOF, cur, p);
  return head.next;
}

Node *new_node(NodeKind kind, Node *lhs, Node *rhs) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = kind;
  node->lhs = lhs;
  node->rhs = rhs;
  return node;
}

void view_node(Node *node) {
  if (!node) {
    return;
  }
  if (node->kind == ND_INT) {
    fprintf(stderr, "%d", node->val);
    return;
  }
  if (node->kind == ND_LVAR) {
    fprintf(stderr, "[%d]", node->offset);
    return;
  }
  if (node->kind == ND_IF) {
    fprintf(stderr, "(if ");
    view_node(node->lhs);
    fprintf(stderr, " ");
    view_node(node->rhs->lhs);
    fprintf(stderr, " ");
    view_node(node->rhs->rhs);
    fprintf(stderr, ")");
    return;
  }
  if (node->kind == ND_BLOCK) {
    fprintf(stderr, "(block");
    for (Node *s = node->rhs; s; s = s->rhs) {
      fprintf(stderr, " ");
      view_node(s->lhs);
    }
    fprintf(stderr, ")");
    return;
  }
  fprintf(stderr, "(");
  switch (node->kind) {
  case ND_RETURN:
    fprintf(stderr, "return");
    break;
  case ND_ASSIGN:
    fprintf(stderr, "=");
    break;
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
  case ND_EQU:
    fprintf(stderr, "==");
    break;
  case ND_NEQ:
    fprintf(stderr, "!=");
    break;
  case ND_LES:
    fprintf(stderr, "<");
    break;
  case ND_LEQ:
    fprintf(stderr, "<=");
    break;
  default:
    fprintf(stderr, "fatal error in view_node()\n");
    exit(1);
  }
  fprintf(stderr, " ");
  view_node(node->lhs);
  fprintf(stderr, " ");
  view_node(node->rhs);
  fprintf(stderr, ")");
}

Node *new_node_ident(int offset) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = ND_LVAR;
  node->offset = offset;
  return node;
}

Node *new_node_num(int val) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = ND_INT;
  node->val = val;
  return node;
}

void program();
Node *stmt();
Node *expr();
Node *assign();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *unary();
Node *primary();

void parse() {
  LOCAL_VARS = new_lvar(NULL, "", 0, 0);
  program();
}

Node *code[100];
void program() {
  int i = 0;
  while (!at_eof()) {
    code[i++] = stmt();
  }
  code[i] = NULL;
}

Node *stmt() {
  if (consume_tk(TK_IF)) {
    expect("(");
    Node *cond = expr();
    expect(")");
    Node *then = stmt();
    /*
       *-------*------[els of NULL]
       |       |
      [cond]  [then]
    */
    Node *nif = new_node(ND_IF, cond, new_node(ND_INVALID, then, NULL));
    if (consume_tk(TK_ELSE)) {
      Node *els = stmt();
      nif->rhs->rhs = els;
    }
    return nif;
  }
  Node *node;
  if (consume("{")) {
    node = new_node(ND_BLOCK, NULL, NULL);
    Node *cur = node;
    while (!consume("}")) {
      Node *s = stmt();
      cur->rhs = new_node(ND_INVALID, s, NULL);
      cur = cur->rhs;
    }
    return node;
  }
  if (consume_tk(TK_RETURN)) {
    Node *e = expr();
    node = new_node(ND_RETURN, e, NULL);
  } else {
    node = expr();
  }
  expect(";");
  return node;
}

Node *expr() {
  return assign();
}

Node *assign() {
  Node *node = equality();
  if (consume("=")) {
    node = new_node(ND_ASSIGN, node, assign());
  }
  return node;
}

Node *equality() {
  Node *node = relational();
  for (;;) {
    if (consume("==")) {
      node = new_node(ND_EQU, node, relational());
    } else if (consume("!=")) {
      node = new_node(ND_NEQ, node, relational());
    } else {
      return node;
    }
  }
}

Node *relational() {
  Node *node = add();
  for (;;) {
    if (consume("<=")) {
      node = new_node(ND_LEQ, node, add());
    } else if (consume("<")) {
      node = new_node(ND_LES, node, add());
    } else if (consume(">=")) {
      node = new_node(ND_LEQ, add(), node);
    } else if (consume(">")) {
      node = new_node(ND_LES, add(), node);
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
  Token *tok = consume_ident();
  if (tok) {
    LVar *lvar = find_lvar(tok);
    if (lvar) {
      return new_node_ident(lvar->offset);
    }
    // make a new variable
    lvar = new_lvar(LOCAL_VARS, tok->str, tok->len, LOCAL_VARS->offset+8);
    LOCAL_VARS = lvar;
    return new_node_ident(lvar->offset);
  }
  return new_node_num(expect_number());
}
