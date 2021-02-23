#include <stdio.h>
#include "tocc.h"

void push_val(int val) {
  printf("  mov r3, #%d\n", val);
  printf("  str r3, [sp, #-4]!\n");
}

// 左辺値のアドレスをスタックに1個積む
void gen_lval(Node* node) {
  if (node->kind == ND_LVAR) {
    printf("  mov rax, rbp\n");
    printf("  sub rax, %d\n", node->offset);
    printf("  push rax\n");
  } else if (node->kind == ND_GVAR) {
    printf("  push offset %.*s\n", node->name_len, node->name);
  } else {
    error("代入の左辺値が変数ではありません");
  }
}

// 値と代入先アドレスをpopして代入する
// 代入した値をスタックに積む
void gen_assign(Type* lhs_type) {
  printf("  pop rdi\n");
  printf("  pop rax\n");
  switch (lhs_type->ty) {
    case CHAR:
      printf("  mov BYTE PTR [rax], dil\n");
      break;
    case INT:
      printf("  mov DWORD PTR [rax], edi\n");
      break;
    case PTR:
      printf("  mov [rax], rdi\n");
      break;
    case ARRAY:
      error("配列型への代入はできません");
      break;
  }
  // 代入した値をスタックに積む
  printf("  push rdi\n");
}

char* initializer_label(size_t size) {
  switch (size) {
    case 1:
      return ".byte";
    case 4:
      return ".long";
    case 8:
      return ".quad";
    default:
      error("未対応のサイズ[%zu]です", size);
  }
  return NULL;
}

void gen(Node* node) {
  switch (node->kind) {
    case ND_FUNCTION:
      printf("  .text\n");
      printf("  .global %.*s\n", node->name_len, node->name);
      printf("  .type %.*s, %%function\n", node->name_len, node->name);
      printf("%.*s:\n", node->name_len, node->name);
      // プロローグ
      printf("  push {fp, lr}\n");
      printf("  add fp, sp, #4\n");
      // 変数26個分の領域を確保する
      printf("  sub sp, sp, #208\n");

      // 引数をレジスタから変数へ移動
      for (int i = 0; i < node->childs->size; i++) {
        // 左辺(代入先)のアドレスをスタックに積む
        Node* lhs = node->childs->array[i];
        gen_lval(lhs);
        // 右辺の値をスタックに積む
        gen_lval(node->childs->array[i]);
        switch (i) {
          case 0:
            printf("  push rdi\n");
            break;
          case 1:
            printf("  push rsi\n");
            break;
          case 2:
            printf("  push rdx\n");
            break;
          case 3:
            printf("  push rcx\n");
            break;
          case 4:
            printf("  push r8\n");
            break;
          case 5:
            printf("  push r9\n");
            break;
          default:
            error("引数7個以上は未対応です");
        }
        // 代入実行
        gen_assign(node->childs->array[i]->type);
        // 代入結果をスタックから除く
        printf("  pop rax\n");
      }

      // 関数の中身
      gen(node->lhs);

      return;
    case ND_GVAR_DEF: {
      int size = get_type_size(node->type);
      printf("  .global %.*s\n", node->name_len, node->name);
      printf("  .data\n");
      printf("  .align 4\n");  // TODO
      printf("  .type %.*s, @object\n", node->name_len, node->name);
      printf("  .size %.*s, %d\n", node->name_len, node->name, size);
      printf("%.*s:\n", node->name_len, node->name);
      // 初期値の設定
      Node* initializer = node->lhs;
      if (initializer) {
        if (initializer->kind == ND_INITS) {
          for (int i = 0; i < initializer->childs->size; ++i) {
            Node* elem = initializer->childs->array[i];
            int elem_size = get_type_size(node->type->ptr_to);
            if (elem->kind == ND_NUM) {
              printf("  %s %d\n", initializer_label(elem_size), elem->val);
            } else {
              error("定数以外は未対応です");
            }
          }
        } else if (initializer->kind == ND_NUM) {
          printf("  %s %d\n", initializer_label(size), initializer->val);
        } else if (initializer->kind == ND_STRING) {
          String* sval = initializer->sval;
          printf("  .ascii \"%.*s\\0\"\n", sval->len, sval->str);
        } else {
          error("定数以外は未対応です");
        }
      } else {
        printf("  .zero %d\n", size);
      }
      return;
    }
    case ND_LVAR_DEF: {
      // 初期値の設定
      Node* initializer = node->rhs;
      if (initializer) {
        if (initializer->kind == ND_INITS) {
          Type* array_to = node->lhs->type->ptr_to;
          for (int i = 0; i < node->lhs->type->array_size; ++i) {
            int size = get_type_size(array_to);
            // 左辺値のアドレスをpush
            printf("  mov rax, rbp\n");
            printf("  sub rax, %d\n", node->lhs->offset);
            printf("  add rax, %d\n", i * size);
            printf("  push rax\n");
            // 右辺の値をpush
            if (i < initializer->childs->size) {
              gen(initializer->childs->array[i]);
            } else {
              printf("  push 0\n");
            }
            //代入
            gen_assign(array_to);
          }
        } else if (initializer->kind == ND_STRING) {
          for (int i = 0; i < node->lhs->type->array_size; ++i) {
            // 左辺値のアドレスをpush
            printf("  mov rax, rbp\n");
            printf("  sub rax, %d\n", node->lhs->offset);
            printf("  add rax, %d\n", i);
            printf("  push rax\n");
            // 右辺の値をpush
            if (i < initializer->sval->len) {
              printf("  push %hhd\n", initializer->sval->str[i]);
            } else {
              printf("  push 0\n");
            }
            //代入
            gen_assign(node->lhs->type->ptr_to);
          }
        } else {
          gen_lval(node->lhs);
          gen(initializer);
          gen_assign(node->lhs->type);
        }
      }
      return;
    }
    case ND_NUM:
      // 数値を1個スタックに積む
      push_val(node->val);
      return;
    case ND_STRING:
      // 数値を1個スタックに積む
      printf("  push offset .LC%d\n", node->label);
      return;
    case ND_LVAR:
    case ND_GVAR:
      // 変数の値をスタックに積む
      //   変数のアドレスをスタックに積む
      gen_lval(node);
      //   変数のアドレスから値に積み替える
      printf("  pop rax\n");
      switch (node->type->ty) {
        case CHAR:
          printf("  movsx eax, BYTE PTR [rax]\n");
          break;
        case INT:
          printf("  mov eax, DWORD PTR [rax]\n");
          break;
        case PTR:
          printf("  mov rax, [rax]\n");
          break;
        case ARRAY:
          // アドレスをそのままスタックに積む
          break;
      }
      printf("  push rax\n");
      return;
    case ND_ASSIGN:
      // 左辺(代入先)のアドレスをスタックに積む
      if (node->lhs->kind == ND_DEREF) {
        // DEREF対象の変数が目的(代入先)のアドレス
        gen(node->lhs->lhs);
      } else {
        gen_lval(node->lhs);
      }
      // 右辺の値をスタックに積む
      gen(node->rhs);

      // 代入実行
      gen_assign(node->lhs->type);
      return;
    case ND_ADDR:
      gen_lval(node->lhs);
      return;
    case ND_DEREF:
      gen(node->lhs);
      printf("  pop rax\n");
      switch (node->lhs->type->ptr_to->ty) {
        case CHAR:
          printf("  movsx eax, BYTE PTR [rax]\n");
          break;
        case INT:
          printf("  mov eax, DWORD PTR [rax]\n");
          break;
        case PTR:
          printf("  mov rax, [rax]\n");
          break;
        case ARRAY:
          // アドレスをそのままスタックに積む
          break;
      }
      printf("  push rax\n");
      return;
    case ND_RETURN:
      // 左の値をスタックに積む
      gen(node->lhs);
      // スタックをr0に詰めてret
      printf("  pop {r0}\n");
      // エピローグ
      printf("  sub sp, fp, #4\n");
      printf("  pop {fp, lr}\n");
      printf("  bx lr\n");
      return;
    case ND_IF:
      // 条件式の結果をスタックに積む
      gen(node->lhs);
      // 分岐
      printf("  pop rax\n");
      printf("  cmp rax, 0\n");
      printf("  je .Lelse%d\n", node->label);
      // then
      gen(node->rhs);
      printf("  jmp .Lend%d\n", node->label);
      printf(".Lelse%d:\n", node->label);
      // else
      if (node->else_stmt) {
        gen(node->else_stmt);
      }
      printf(".Lend%d:\n", node->label);
      return;
    case ND_WHILE:
      printf(".Lbegin%d:\n", node->label);
      // 条件式の結果をスタックに積む
      gen(node->lhs);
      // 分岐
      printf("  pop rax\n");
      printf("  cmp rax, 0\n");
      printf("  je .Lend%d\n", node->label);
      // ループの中身
      gen(node->rhs);
      printf("  jmp .Lbegin%d\n", node->label);
      printf(".Lend%d:\n", node->label);
      return;
    case ND_FOR:
      // 初期化式
      gen(node->lhs);
      printf("  pop rax\n");  // スタックに式の値が残っている

      printf(".Lbegin%d:\n", node->label);
      // 条件式の結果をスタックに積む
      gen(node->rhs);
      // 分岐
      printf("  pop rax\n");
      printf("  cmp rax, 0\n");
      printf("  je .Lend%d\n", node->label);
      // 中身
      gen(node->content_stmt);
      // 後処理(forの3項目)
      gen(node->post_expr);
      printf("  pop rax\n");  // スタックに式の値が残っている

      printf("  jmp .Lbegin%d\n", node->label);
      printf(".Lend%d:\n", node->label);
      return;
    case ND_BLOCK:
      for (int i = 0; i < node->childs->size; i++) {
        gen(node->childs->array[i]);
        // 式の評価結果としてスタックに1つの値が残っている
        // はずなので、スタックが溢れないようにポップしておく
        printf("  pop {r0}\n");
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

      printf("  mov rax, 0\n");
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
  // 左辺と右辺をそれぞれスタックに積む
  gen(node->lhs);
  gen(node->rhs);

  // 2項演算子はスタックから2個値(左辺と右辺)をpopし、結果を1個pushする
  printf("  pop {r3}\n");
  printf("  pop {r2}\n");

  switch (node->kind) {
    case ND_ADD:
      printf("  add r3, r2, r3\n");
      break;
    case ND_SUB:
      printf("  sub r3, r2, r3\n");
      break;
    case ND_MUL:
      printf("  mul r3, r2, r3\n");
      break;
    case ND_DIV:
      // 割り算命令は無いので、組み込み関数の呼び出しに置き換え
      printf("  mov r1, r3\n");
      printf("  mov r0, r2\n");
      printf("  bl __aeabi_idiv\n");
      printf("  mov r3, r0\n");
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
  // 2項演算子の結果をpush
  printf("  push {r3}\n");
}

void gen_strings() {
  for (int i = 0; i < all_strings->size; ++i) {
    printf(".LC%d:\n", i);
    String* str = all_strings->array[i];
    printf("  .string \"%.*s\"\n", str->len, str->str);
  }
}
