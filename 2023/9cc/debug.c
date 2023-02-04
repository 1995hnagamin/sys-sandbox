#include <stdio.h>
#include <stdlib.h>
#include "9cc.h"

void view_token(Token *tok) {
  fprintf(stderr, "%.*s (%d)\n", tok->len, tok->str, tok->len);
}

void view_type(Type *type) {
  if (!type) {
    return;
  }
  switch (type->kind) {
  case TY_INVALID:
    fprintf(stderr, "<invalid typ>");
    break;
  case TY_CHAR:
    fprintf(stderr, "char");
    break;
  case TY_INT:
    fprintf(stderr, "int");
    break;
  case TY_ARRAY:
    fprintf(stderr, "(array %zu ", type->array_size);
    view_type(type->ptr_to);
    fprintf(stderr, ")");
    break;
  case TY_FN:
    fprintf(stderr, "(function ");
    view_type(type->ptr_to);
    fprintf(stderr, ")");
    break;
  case TY_PTR:
    fprintf(stderr, "(pointer ");
    view_type(type->ptr_to);
    fprintf(stderr, ")");
  }
}


void view_node(Node *node) {
  if (!node) {
    return;
  }
  if (node->kind == ND_INT) {
    fprintf(stderr, "%d", node->val);
    return;
  }
  if (node->kind == ND_STR) {
    fprintf(stderr, "\"%.*s\"", node->tok->len, node->tok->str);
    return;
  }
  if (node->kind == ND_LVAR) {
    fprintf(stderr, "[%d]", node->lvar->offset);
    return;
  }
  if (node->kind == ND_GVAR) {
    fprintf(stderr, "[global:%.*s]", node->gvar->len, node->gvar->name);
    return;
  }
  if (node->kind == ND_FNDEF) {
    fprintf(stderr, "(fndef ");
    fprintf(stderr, "%.*s ", node->tok->len, node->tok->str);
    fprintf(stderr, "(");
    for (Node *param = node->rhs; param; param = param->rhs) {
      view_node(param->lhs);
    }
    fprintf(stderr, ") ");
    view_node(node->lhs);
    fprintf(stderr, ")");
    return;
  }
  if (node->kind == ND_DECL) {
    fprintf(stderr, "(decl ");
    fprintf(stderr, "%.*s ", node->tok->len, node->tok->str);
    view_type(node->ty);
    fprintf(stderr, ")");
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
  if (node->kind == ND_FNCALL) {
    fprintf(stderr, "(call ");
    fprintf(stderr, "%.*s", node->tok->len, node->tok->str);
    for (Node *n = node->rhs; n; n = n->rhs) {
      fprintf(stderr, " ");
      view_node(n->lhs);
    }
    fprintf(stderr, ")");
    return;
  }
  if (node->kind == ND_ADDR) {
    fprintf(stderr, "(address ");
    view_node(node->lhs);
    fprintf(stderr, ")");
    return;
  }
  if (node->kind == ND_DEREF) {
    fprintf(stderr, "(deref ");
    view_node(node->lhs);
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
  fprintf(stderr, " [");
  view_type(node->ty);
  fprintf(stderr, "] ");
  view_node(node->lhs);
  fprintf(stderr, " ");
  view_node(node->rhs);
  fprintf(stderr, ")");
}
