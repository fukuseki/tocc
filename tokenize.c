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

  // locが含まれている行の開始地点と終了地点を取得
  char* line = loc;
  while (user_input < loc && line[-1] != '\n') {
    line--;
  }
  char* end = loc;
  while (*end != '\n') {
    end++;
  }

  // 見つかった行が何行目なのかを調べる
  int line_num = 1;
  for (char* p = user_input; p < line; p++) {
    if (*p == '\n') {
      line_num++;
    }
  }

  // 見つかった行を、ファイル名と行番号と一緒に表示
  int indent = fprintf(stderr, "%s:%d: ", filename, line_num);
  fprintf(stderr, "%.*s\n", (int)(end - line), line);

  // エラー箇所を"^"で指し示して、エラーメッセージを表示
  int pos = loc - line + indent;
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

// 次のトークンが期待している型か
bool lookahead_kind(TokenKind kind) {
  return token->kind == kind;
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

String* expect_string() {
  if (token->kind != TK_STRING) {
    error_at(token->str, "文字列ではありません");
  }
  String* str = new_string(token->str, token->len);
  token = token->next;
  return str;
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

    // 文字列リテラル
    if (*p == '"') {
      p++;
      cur = new_token(TK_STRING, cur, p);
      int len = 0;
      while (p[len] != '"') {
        len++;
      }
      p += len + 1;
      cur->len = len;
      continue;
    }

    error_at(p, "トークナイズできません");
  }

  new_token(TK_EOF, cur, p);
  return head.next;
}
