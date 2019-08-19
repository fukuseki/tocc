#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include "tocc.h"

// エラーを報告するための関数
// printfと同じ引数を取る
void error(char* fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

int main(int argc, char** argv) {
  if (argc != 2) {
    error("引数の個数が正しくありません");
  }

  // トークナイズしてパースする
  user_input = argv[1];
  token = tokenize(user_input);
  Node* node = expr();

  // アセンブリの先頭部分を出力
  printf(".intel_syntax noprefix\n");
  printf(".global main\n");
  printf("main:\n");

  // 抽象構文木を下りながらコード生成
  gen(node);

  // スタックトップに式全体の値が残っているはずなので
  // それをRAXにロードして関数全体の返り値とする
  printf("  pop rax\n");
  printf("  ret\n");
  return 0;
}
