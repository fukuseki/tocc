#include <stdio.h>
#include "tocc.h"

void gen_lval(Node* node) {
  if (node->kind != ND_LVAR) {
    error("代入の左辺値が変数ではありません");
  }

  printf("  mov rax, rbp\n");
  printf("  sub rax, %d\n", node->offset);
  printf("  push rax\n");
}

void gen(Node* node) {
  switch (node->kind) {
    case ND_FUNCTION:
      printf("%.*s:\n", node->name_len, node->name);
      // プロローグ
      // 変数26個分の領域を確保する
      printf("  push rbp\n");
      printf("  mov rbp, rsp\n");
      printf("  sub rsp, 208\n");

      // 引数
      for (int i = 0; i < node->childs->size; i++) {
        gen_lval(node->childs->array[i]);
      }
      // 引数をレジスタからスタックへ移動
      if (7 <= node->childs->size) {
        error("引数7個以上は未対応です");
      }
      if (6 <= node->childs->size) {
        printf("  pop rax\n");
        printf("  mov [rax], r9\n");
      }
      if (5 <= node->childs->size) {
        printf("  pop rax\n");
        printf("  mov [rax], r8\n");
      }
      if (4 <= node->childs->size) {
        printf("  pop rax\n");
        printf("  mov [rax], rcx\n");
      }
      if (3 <= node->childs->size) {
        printf("  pop rax\n");
        printf("  mov [rax], rdx\n");
      }
      if (2 <= node->childs->size) {
        printf("  pop rax\n");
        printf("  mov [rax], rsi\n");
      }
      if (1 <= node->childs->size) {
        printf("  pop rax\n");
        printf("  mov [rax], rdi\n");
      }

      // 関数の中身
      gen(node->lhs);

      // エピローグ
      // 最後の式の結果がRAXに残っているのでそれが返り値になる
      printf("  mov rsp, rbp\n");
      printf("  pop rbp\n");
      printf("  ret\n");
    case ND_NUM:
      printf("  push %d\n", node->val);
      return;
    case ND_LVAR:
      gen_lval(node);
      printf("  pop rax\n");
      printf("  mov rax, [rax]\n");
      printf("  push rax\n");
      return;
    case ND_ASSIGN:
      gen_lval(node->lhs);
      gen(node->rhs);

      printf("  pop rdi\n");
      printf("  pop rax\n");
      printf("  mov [rax], rdi\n");
      printf("  push rdi\n");
      return;
    case ND_RETURN:
      gen(node->lhs);
      printf("  pop rax\n");
      printf("  mov rsp, rbp\n");
      printf("  pop rbp\n");
      printf("  ret\n");
      return;
    case ND_IF:
      gen(node->lhs);
      printf("  pop rax\n");
      printf("  cmp rax, 0\n");
      printf("  je .Lelse%d\n", node->label);
      gen(node->rhs);
      printf("  jmp .Lend%d\n", node->label);
      printf(".Lelse%d:\n", node->label);
      if (node->else_stmt) {
        gen(node->else_stmt);
      }
      printf(".Lend%d:\n", node->label);
      return;
    case ND_WHILE:
      printf(".Lbegin%d:\n", node->label);
      gen(node->lhs);
      printf("  pop rax\n");
      printf("  cmp rax, 0\n");
      printf("  je .Lend%d\n", node->label);
      gen(node->rhs);
      printf("  jmp .Lbegin%d\n", node->label);
      printf(".Lend%d:\n", node->label);
      return;
    case ND_FOR:
      gen(node->lhs);
      printf(".Lbegin%d:\n", node->label);
      gen(node->rhs);
      printf("  pop rax\n");
      printf("  cmp rax, 0\n");
      printf("  je .Lend%d\n", node->label);
      gen(node->content_stmt);
      gen(node->post_expr);
      printf("  jmp .Lbegin%d\n", node->label);
      printf(".Lend%d:\n", node->label);
      return;
    case ND_BLOCK:
      for (int i = 0; i < node->childs->size; i++) {
        gen(node->childs->array[i]);
        // 式の評価結果としてスタックに1つの値が残っている
        // はずなので、スタックが溢れないようにポップしておく
        printf("  pop rax\n");
      }
      return;
    case ND_CALL:
      // 引数をスタックに積む
      for (int i = 0; i < node->childs->size; i++) {
        gen(node->childs->array[i]);
      }
      // 引数をスタックからレジスタに載せる
      if (7 <= node->childs->size) {
        error("引数7個以上は未対応です");
      }
      if (6 <= node->childs->size) {
        printf("  pop r9\n");
      }
      if (5 <= node->childs->size) {
        printf("  pop r8\n");
      }
      if (4 <= node->childs->size) {
        printf("  pop rcx\n");
      }
      if (3 <= node->childs->size) {
        printf("  pop rdx\n");
      }
      if (2 <= node->childs->size) {
        printf("  pop rsi\n");
      }
      if (1 <= node->childs->size) {
        printf("  pop rdi\n");
      }
      // RSPを8の倍数から16の倍数になるように調整
      printf("  mov rax, rsp\n");
      printf("  add rax, 8\n");  // 調整値のpush分
      printf("  and rax, 8\n");
      printf("  sub rsp, rax\n");
      printf("  push rax\n");

      printf("  call %.*s\n", node->name_len, node->name);
      // 調整分を戻す
      printf("  pop rdi\n");
      printf("  add rsp, rdi\n");

      printf("  push rax\n");
      return;
    default:
      break;
  }

  // 2項演算子いろいろ
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
    case ND_EQ:
      printf("  cmp rax, rdi\n");
      printf("  sete al\n");
      printf("  movzb rax, al\n");
      break;
    case ND_NE:
      printf("  cmp rax, rdi\n");
      printf("  setne al\n");
      printf("  movzb rax, al\n");
      break;
    case ND_LT:
      printf("  cmp rax, rdi\n");
      printf("  setl al\n");
      printf("  movzb rax, al\n");
      break;
    case ND_LE:
      printf("  cmp rax, rdi\n");
      printf("  setle al\n");
      printf("  movzb rax, al\n");
      break;
    default:
      error("予期しないkind=%d", node->kind);
  }

  printf("  push rax\n");
}
