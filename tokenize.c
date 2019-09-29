#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

// 入力プログラム
char* user_input;

// エラー箇所を報告する
void error_at(char* loc, char* fmt, ...) {
  va_list ap;
  va_start(ap, fmt);

  int pos = loc - user_input;
  fprintf(stderr, "%s\n", user_input);
  fprintf(stderr, "%*s", pos, "");  // pos個の空白を出力
  fprintf(stderr, "^ ");
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

bool is_alpha(char c) {
  return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || (c == '_');
}

bool is_alnum(char c) {
  return is_alpha(c) || ('0' <= c && c <= '9');
}

// 現在注目しているトークン
Token* token;

// 次のトークンが期待している記号か
bool lookahead(char* op) {
  return (token->kind == TK_RESERVED && strlen(op) == token->len &&
          !memcmp(token->str, op, token->len));
}

// 次のトークンが期待している記号のときには、トークンを1つ読み進めて
// 真を返す。それ以外の場合には偽を返す。
bool consume(char* op) {
  if (token->kind != TK_RESERVED || strlen(op) != token->len ||
      memcmp(token->str, op, token->len)) {
    return false;
  }
  token = token->next;
  return true;
}

Token* consume_ident() {
  if (token->kind != TK_IDENT) {
    return NULL;
  }
  Token* cur = token;
  token = token->next;
  return cur;
}

// 次のトークンが期待している記号のときには、トークンを1つ読み進める。
// それ以外の場合にはエラーを報告する。
void expect(char* op) {
  if (token->kind != TK_RESERVED || strlen(op) != token->len ||
      memcmp(token->str, op, token->len)) {
    error_at(token->str, "'%s'ではありません", op);
  }
  token = token->next;
}

// 次のトークンが数値の場合、トークンを1つ読み進めてその数値を返す。
// それ以外の場合にはエラーを報告する。
int expect_number() {
  if (token->kind != TK_NUM) {
    error_at(token->str, "数ではありません");
  }
  int val = token->val;
  token = token->next;
  return val;
}

bool at_eof() {
  return token->kind == TK_EOF;
}

// 新しいトークンを作成してcurに繋げる
Token* new_token(TokenKind kind, Token* cur, char* str) {
  Token* tok = calloc(1, sizeof(Token));
  tok->kind = kind;
  tok->str = str;
  cur->next = tok;
  return tok;
}

// 予約語が含まれるか検索する
// 含まれる場合はヒットした予約語を返す
const char* find_reserved(char* p) {
  const char* reserved_words[] = {
      "return", "if", "else", "while", "for", "int", "char", "sizeof",
  };

  for (int i = 0; i < sizeof(reserved_words) / sizeof(reserved_words[0]); i++) {
    int len = strlen(reserved_words[i]);
    if (strncmp(p, reserved_words[i], len) == 0 && !is_alnum(p[len])) {
      return reserved_words[i];
    }
  }
  return NULL;
}

// 入力文字列pをトークナイズしてそれを返す
Token* tokenize() {
  char* p = user_input;
  Token head;
  head.next = NULL;
  Token* cur = &head;

  while (*p) {
    // 空白文字をスキップ
    if (isspace(*p)) {
      p++;
      continue;
    }

    // 予約語
    const char* reserved = find_reserved(p);
    if (reserved) {
      int len = strlen(reserved);
      cur = new_token(TK_RESERVED, cur, p);
      p += len;
      cur->len = len;
      continue;
    }

    // 2文字の記号
    if (memcmp(p, "==", 2) == 0 || memcmp(p, "!=", 2) == 0 ||
        memcmp(p, "<=", 2) == 0 || memcmp(p, ">=", 2) == 0) {
      cur = new_token(TK_RESERVED, cur, p);
      p += 2;
      cur->len = 2;
      continue;
    }

    // 1文字の記号
    if (strchr("+-*/()<>=;{},&[]", *p)) {
      cur = new_token(TK_RESERVED, cur, p++);
      cur->len = 1;
      continue;
    }

    // 識別子
    if (is_alpha(*p)) {
      cur = new_token(TK_IDENT, cur, p);
      int len = 1;
      // 2文字目以降は英数字
      while (is_alnum(p[len])) {
        len++;
      }
      cur->len = len;
      p += len;
      continue;
    }

    if (isdigit(*p)) {
      cur = new_token(TK_NUM, cur, p);
      cur->val = strtol(p, &p, 10);
      continue;
    }

    error_at(p, "トークナイズできません");
  }

  new_token(TK_EOF, cur, p);
  return head.next;
}
