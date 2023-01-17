#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "9cc.h"

void get_lval(Node *node) {
  if (node->kind != ND_LVAR) {
    error("the lhs of assignment expression should be a variable");
  }
  printf("  mov rax, rbp\n");
  printf("  sub rax, %d\n", node->offset);
  printf("  push rax\n");
}

int GENSYM_IDX;
int gensym() {
  return ++GENSYM_IDX;
}

void gen_if_stmt(Node *node) {
  int end_label = gensym();
  gen(node->lhs);
  printf("  pop rax\n");
  printf("  cmp rax, 0\n");
  if (node->rhs->rhs) {
    int else_label = gensym();
    printf("  je .LElse%d\n", else_label);
    gen(node->rhs->lhs); // then
    printf("  jmp .LEnd%d\n", end_label);
    printf(".LElse%d:\n", else_label);
    gen(node->rhs->rhs); // else
  } else {
    printf("  je .LEnd%d\n", end_label);
    gen(node->rhs->lhs); // then
  }
  printf(".LEnd%d:\n", end_label);
  return;
}

void gen_fn_call(Node *call) {
  char *reg[6] = {"rdi", "rsi", "rdx", "rcx", "r8d", "r9d"};
  Node *arg = call->rhs;
  for (int i = 0; i < 6 && arg; ++i, arg = arg->rhs) {
    gen(arg->lhs);
    printf("  pop %s\n", reg[i]);
  }
  printf("  call _%.*s\n", call->tok->len, call->tok->str);
}

void gen(Node *node) {
  switch (node->kind) {
  case ND_FNDEF:
    printf("_%.*s:\n", node->tok->len, node->tok->str);
    printf("  push rbp\n");
    printf("  mov rbp, rsp\n");
    printf("  sub rsp, 208\n");
    gen(node->lhs); // function body
    printf("  mov rsp, rbp\n");
    printf("  pop rbp\n");
    printf("  ret\n");
    return;
  case ND_INT:
    printf("  push %d\n", node->val);
    return;
  case ND_LVAR:
    get_lval(node);
    printf("  pop rax\n");
    printf("  mov rax, [rax]\n");
    printf("  push rax\n");
    return;
  case ND_IF:
    gen_if_stmt(node);
    return;
  case ND_ASSIGN:
    get_lval(node->lhs);
    gen(node->rhs);
    printf("  pop rdi\n");
    printf("  pop rax\n");
    printf("  mov [rax], rdi\n");
    printf("  push rdi\n");
    return;
  case ND_FNCALL:
    gen_fn_call(node);
    break;
  case ND_RETURN:
    gen(node->lhs);
    printf("  pop rax\n");
    printf("  mov rsp, rbp\n");
    printf("  pop rbp\n");
    printf("  ret\n");
    return;
  case ND_BLOCK:
    for (Node *s = node->rhs; s; s = s->rhs) {
      gen(s->lhs);
      printf("  pop rax\n");
    }
    return;
  case ND_ADD:
    gen(node->lhs);
    gen(node->rhs);
    printf("  pop rdi\n");
    printf("  pop rax\n");
    printf("  add rax, rdi\n");
    printf("  push rax\n");
    break;
  case ND_SUB:
    gen(node->lhs);
    gen(node->rhs);
    printf("  pop rdi\n");
    printf("  pop rax\n");
    printf("  sub rax, rdi\n");
    printf("  push rax\n");
    break;
  case ND_MUL:
    gen(node->lhs);
    gen(node->rhs);
    printf("  pop rdi\n");
    printf("  pop rax\n");
    printf("  imul rax, rdi\n");
    printf("  push rax\n");
    break;
  case ND_DIV:
    gen(node->lhs);
    gen(node->rhs);
    printf("  pop rdi\n");
    printf("  pop rax\n");
    printf("  cqo\n");
    printf("  idiv rdi\n");
    printf("  push rax\n");
    break;
  case ND_EQU:
    gen(node->lhs);
    gen(node->rhs);
    printf("  pop rdi\n");
    printf("  pop rax\n");
    printf("  cmp rax, rdi\n");
    printf("  sete al\n");
    printf("  movzx rax, al\n");
    printf("  push rax\n");
    break;
  case ND_NEQ:
    gen(node->lhs);
    gen(node->rhs);
    printf("  pop rdi\n");
    printf("  pop rax\n");
    printf("  cmp rax, rdi\n");
    printf("  setne al\n");
    printf("  movzx rax, al\n");
    printf("  push rax\n");
    break;
  case ND_LES:
    gen(node->lhs);
    gen(node->rhs);
    printf("  pop rdi\n");
    printf("  pop rax\n");
    printf("  cmp rax, rdi\n");
    printf("  setl al\n");
    printf("  movzx rax, al\n");
    printf("  push rax\n");
    break;
  case ND_LEQ:
    gen(node->lhs);
    gen(node->rhs);
    printf("  pop rdi\n");
    printf("  pop rax\n");
    printf("  cmp rax, rdi\n");
    printf("  setle al\n");
    printf("  movzx rax, al\n");
    printf("  push rax\n");
    break;
  case ND_INVALID:
    fprintf(stderr, "fatal error in gen()\n");
    exit(1);
  }
}
