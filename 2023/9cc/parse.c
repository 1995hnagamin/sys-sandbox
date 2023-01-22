#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "9cc.h"

bool is_operator(char *op, Token *token) {
  return token->kind == TK_RESERVED
    && (int)(strlen(op)) == token->len
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

void expect_tk(TokenKind tk) {
  if (CUR_TOKEN->kind != tk) {
    error_at(CUR_TOKEN->str, "unexpected token");
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
  lvar->ty = NULL;
  return lvar;
}

Node *new_node(NodeKind kind, Node *lhs, Node *rhs) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = kind;
  node->lhs = lhs;
  node->rhs = rhs;
  node->ty = NULL;
  return node;
}

Node *new_node_ident(LVar *lvar) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = ND_LVAR;
  node->lvar = lvar;
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
Node *declaration();
Node *declarator(Type *ty_spec);
Node *expr();
Node *assign();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *unary();
Node *primary();

void parse() {
  program();
}

Node *code[100];
void program() {
  int i = 0;
  while (consume_tk(TK_INT)) {
    Token *fn_name = consume_ident();
    LOCAL_VARS = new_lvar(NULL, "", 0, 0);
    Node *def = new_node(ND_FNDEF, NULL, NULL);
    def->tok = fn_name;
    expect("(");
    Node *node = def;
    while (!consume(")")) {
      expect_tk(TK_INT);
      Token *name = consume_ident();
      node->rhs = new_node(ND_INVALID, NULL, NULL);
      node = node->rhs;
      LVar *lvar = new_lvar(LOCAL_VARS, name->str, name->len, LOCAL_VARS->offset+8);
      lvar->ty = ty_int();
      LOCAL_VARS = lvar;
      node->lhs = new_node_ident(lvar);
      consume(",");
    }
    Node *body = stmt();
    def->lhs = body;
    code[i++] = def;
    set_type(def);
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
  if (CUR_TOKEN->kind == TK_INT) {
    return declaration();
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

Node *declaration() {
  consume_tk(TK_INT);
  Node *node = declarator(ty_int());
  expect(";");
  return node;
}

Node *declarator(Type *ty_spec) {
  Token *var_name;
  if ((var_name = consume_ident())) {
    LVar *lvar = new_lvar(LOCAL_VARS, var_name->str, var_name->len, LOCAL_VARS->offset+8);
    LOCAL_VARS = lvar;
    Node *id = new_node_ident(lvar);
    Node *node = new_node(ND_DECL, id, NULL);
    node->tok = var_name;
    lvar->ty = ty_spec;
    node->ty = ty_spec;
    return node;
  }
  expect("*");
  Node *node = declarator(new_ty_ptr(ty_spec));
  return node;
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
  if (consume("&")) {
    Node *node = unary();
    return new_node(ND_ADDR, node, NULL);
  }
  if (consume("*")) {
    Node *node = unary();
    return new_node(ND_DEREF, node, NULL);
  }
  Token *tok = consume_ident();
  if (!tok) {
    // should be a number
    return new_node_num(expect_number());
  }
  if (consume("(")) {
    // shoule be a function call
    Node *call = new_node(ND_FNCALL, NULL, NULL);
    call->tok = tok;
    Node *node = call;
    while (!consume(")")) {
      node->rhs = new_node(ND_INVALID, expr(), NULL);
      node = node->rhs;
      consume(",");
    }
    return call;
  }
  LVar *lvar = find_lvar(tok);
  if (lvar) {
    return new_node_ident(lvar);
  }
  // error: variable used before declaration
  error_at(CUR_TOKEN->str, "undeclared variable: %.*s\n", tok->len, tok->str);
  exit(1);
}
