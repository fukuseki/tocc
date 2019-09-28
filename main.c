#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include "tocc.h"

int main(int argc, char** argv) {
  if (argc != 2) {
    error("引数の個数が正しくありません");
  }

  // トークナイズしてパースする
  // 結果はcodeに保存される
  user_input = argv[1];
  token = tokenize();
  program();

  // アセンブリの先頭部分を出力
  printf(".intel_syntax noprefix\n");

  // 先頭の式から順にコード生成
  for (int i = 0; code[i]; i++) {
    gen(code[i]);
  }

  return 0;
}
