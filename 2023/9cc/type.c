#include <stdio.h>
#include <stdlib.h>
#include "9cc.h"

static Type T_CHAR = {
  TY_CHAR,
  NULL,
  .array_size = 0,
  .params = NULL,
};

static Type T_INT = {
  TY_INT,
  NULL,
  .array_size=0,
  .params=NULL,
};

Type *ty_reserved(TypeKind tyk) {
  switch (tyk) {
    case TY_CHAR:
      return &T_CHAR;
    case TY_INT:
      return &T_INT;
    default:
      error("not a fundamental type");
      return NULL;
  }
}

Type *new_ty_array(size_t n, Type *to) {
  Type *t = calloc(1, sizeof(Type));
  t->kind = TY_ARRAY;
  t->array_size = n;
  t->ptr_to = to;
  return t;
}

Type *new_ty_fn(struct Node *params, Type *ret) {
  Type *t = calloc(1, sizeof(Type));
  t->kind = TY_FN;
  t->params = params;
  t->ptr_to = ret;
  return t;
}

Type *new_ty_ptr(Type *to) {
  Type *t = calloc(1, sizeof(Type));
  t->kind = TY_PTR;
  t->ptr_to = to;
  return t;
}


void view_node(Node *node);
void set_type(Node *node) {
  if (!node || node->ty) {
    return;
  }

  switch (node->kind) {
  case ND_FNDEF:
    for (Node *param = node->rhs; param; param = param->rhs) {
      set_type(param->lhs);
    }
    set_type(node->lhs);
    break;
  case ND_DECL:
    set_type(node->lhs);
    node->ty = node->lhs->ty;
    break;
  case ND_INT:
    node->ty = &T_INT;
    break;
  case ND_STR:
    node->ty = new_ty_array(node->tok->len+1, &T_CHAR);
    break;
  case ND_LVAR:
    node->ty = node->lvar->ty;
    break;
  case ND_GVAR:
    node->ty = node->gvar->ty;
    break;
  case ND_IF:
    set_type(node->lhs);
    set_type(node->rhs->lhs);
    set_type(node->rhs->rhs);
    break;
  case ND_WHILE:
    set_type(node->lhs);
    set_type(node->rhs);
    break;
  case ND_ASSIGN:
    set_type(node->lhs);
    set_type(node->rhs);
    node->ty = node->lhs->ty;
    break;
  case ND_FNCALL:
    for (Node *arg = node->rhs; arg; arg = arg->rhs) {
      set_type(arg->lhs);
    }
    node->ty = ty_reserved(TY_INT);
    break;
  case ND_RETURN:
    set_type(node->lhs);
    break;
  case ND_BLOCK:
    for (Node *s = node->rhs; s; s = s->rhs) {
      set_type(s->lhs);
    }
    break;
  case ND_ADD:
  case ND_SUB:
    set_type(node->lhs);
    set_type(node->rhs);
    if (node->lhs->ty->kind == TY_PTR) {
      node->ty = node->lhs->ty;
    } else if (node->rhs->ty->kind == TY_PTR) {
      node->ty = node->rhs->ty;
    } else {
      node->ty = ty_reserved(TY_INT);
    }
    break;
  case ND_MUL:
  case ND_DIV:
    set_type(node->lhs);
    set_type(node->rhs);
    node->ty = ty_reserved(TY_INT);
    break;
  case ND_EQU:
  case ND_NEQ:
  case ND_LES:
  case ND_LEQ:
    set_type(node->lhs);
    set_type(node->rhs);
    node->ty = ty_reserved(TY_INT);
    break;
  case ND_ADDR:
    set_type(node->lhs);
    switch (node->lhs->ty->kind) {
    case TY_ARRAY:
      node->ty = new_ty_ptr(node->lhs->ty->ptr_to);
      break;
    default:
      node->ty = new_ty_ptr(node->lhs->ty);
      break;
    }
    break;
  case ND_DEREF:
    set_type(node->lhs);
    node->ty = node->lhs->ty->ptr_to;
    break;
  case ND_INVALID:
    fprintf(stderr, "fatal error in set_type()\n");
    exit(1);
  }
}
