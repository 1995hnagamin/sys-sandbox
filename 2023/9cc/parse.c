#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "9cc.h"

bool match_reserved_token(char *op, Token *token) {
  return token->kind == TK_RESERVED
    && (int)(strlen(op)) == token->len
    && memcmp(token->str, op, token->len) == 0;
}

bool consume(char *op) {
  if (!match_reserved_token(op, CUR_TOKEN)) {
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
  if (!match_reserved_token(op, CUR_TOKEN)) {
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

int max(int x, int y) {
  return x > y ? x : y;
}

LVar *register_lvar(Token *name, Type *type) {
  switch (type->kind) {
  case TY_FN:
    {
      // just a function declaration
      LVar *lvar = new_lvar(LOCAL_VARS, name->str, name->len, -10000);
      LOCAL_VARS = lvar;
      return lvar;
    }
  default:
    {
      int offset = LOCAL_VARS->offset + max(8, nbytes_type(type));
      LVar *lvar = new_lvar(LOCAL_VARS, name->str, name->len, offset);
      LOCAL_VARS = lvar;
      return lvar;
    }
  }
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

Node *decay(Node *node) {
  set_type(node);
  switch (node->ty->kind) {
  case TY_ARRAY:
    return new_node(ND_ADDR, node, NULL);
  default:
    return node;
  }
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
Node *postfix();
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
      node = new_node(ND_EQU, decay(node), decay(relational()));
    } else if (consume("!=")) {
      node = new_node(ND_NEQ, decay(node), decay(relational()));
    } else {
      return node;
    }
  }
}

Node *declaration_sub() {
  consume_tk(TK_INT);
  Node *node = declarator(ty_int());
  return node;
}

Node *declaration() {
  Node *node = declaration_sub();
  expect(";");
  return node;
}

Node *direct_declarator(Type *spec_type) {
  Token *var_name = consume_ident();
  Node *ident = new_node_ident(NULL);
  ident->tok = var_name;
  Type root = { .kind=TY_PTR, .ptr_to=spec_type };
  Type *tail = &root;
  for (;;) {
      if (consume("[")) {
        int num = expect_number();
        expect("]");
        Type *arr = new_ty_array(num, spec_type);
        tail->ptr_to = arr;
        tail = arr;
      } else if (consume("(")) {
        Node params = { .lhs=NULL, .rhs=NULL,};
        Node *node = &params;
        while (!consume(")")) {
          expect_tk(TK_INT);
          Token *name = consume_ident();
          node->rhs = new_node(ND_INVALID, NULL, NULL);
          node = node->rhs;
          // don't set offsets at this time
          LVar *lvar = new_lvar(LOCAL_VARS, name->str, name->len, -10000);
          lvar->ty = ty_int();
          node->lhs = new_node_ident(lvar);
          consume(",");
        }
        Type *fn = new_ty_fn(params.rhs, spec_type);
        tail->ptr_to = fn;
        tail = fn;
      } else {
        break;
      }
  }
  Type *type = root.ptr_to;
  LVar *lvar = register_lvar(var_name, type);
  lvar->ty = type;
  ident->lvar = lvar;
  Node *decl = new_node(ND_DECL, ident, NULL);
  decl->tok = var_name;
  return decl;
}

/*
  int *x[5] => int *(x[5]) ... [5] * int
 */
Node *declarator(Type *ty_spec) {
  if (consume("*")) {
    Node *node = declarator(new_ty_ptr(ty_spec));
    return node;
  }
  return direct_declarator(ty_spec);
}

Node *relational() {
  Node *node = add();
  for (;;) {
    if (consume("<=")) {
      node = new_node(ND_LEQ, decay(node), decay(add()));
    } else if (consume("<")) {
      node = new_node(ND_LES, decay(node), decay(add()));
    } else if (consume(">=")) {
      node = new_node(ND_LEQ, decay(add()), decay(node));
    } else if (consume(">")) {
      node = new_node(ND_LES, decay(add()), decay(node));
    } else {
      return node;
    }
  }
}

Node *add() {
  Node *node = mul();
  for (;;) {
    if (consume("+")) {
      node = new_node(ND_ADD, decay(node), decay(mul()));
    } else if (consume("-")) {
      node = new_node(ND_SUB, decay(node), decay(mul()));
    } else {
      return node;
    }
  }
}

Node *mul() {
  if (consume_tk(TK_SIZEOF)) {
    Node *exp = unary();
    set_type(exp);
    return new_node_num(nbytes_type(exp->ty));
  }

  Node *node = unary();

  for (;;) {
    if (consume("*")) {
      node = new_node(ND_MUL, decay(node), decay(unary()));
    } else if (consume("/")) {
      node = new_node(ND_DIV, decay(node), decay(unary()));
    } else {
      return node;
    }
  }
}

Node *unary() {
  if (consume("+")) {
    return decay(primary());
  } else if (consume("-")) {
    return new_node(ND_SUB, new_node_num(0), decay(primary()));
  }
  return postfix();
}

Node *postfix() {
  Node *node = primary();
  while (consume("[")) {
    Node *idx = expr();
    expect("]");
    // a[i] => *(a+i)
    node = new_node(ND_DEREF, new_node(ND_ADD, decay(node), decay(idx)), NULL);
    set_type(node);
  }
  return node;
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
    return new_node(ND_DEREF, decay(node), NULL);
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
