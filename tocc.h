#include <ctype.h>
#include <stdbool.h>

void error(char* fmt, ...);
void error_at(char* loc, char* fmt, ...);
extern char* user_input;
extern char* filename;

// 文字列を格納する
typedef struct String String;
struct String {
  char* str;
  int len;
};
String* new_string(char* str, int len);

typedef struct StringVector StringVector;
struct StringVector {
  int size;
  int capacity;
  String** array;
};
StringVector* new_string_vector();
void add_string(StringVector* vec, String* string);

// 全文字列リテラル
StringVector* all_strings;

// トークンの種別
typedef enum {
  TK_RESERVED,  // 記号・予約語
  TK_IDENT,     // 識別子
  TK_NUM,       // 整数トークン
  TK_STRING,    // 文字列リテラル
  TK_EOF,       // 入力の終わりを表すトークン
} TokenKind;

typedef struct Token Token;

// トークン型
struct Token {
  TokenKind kind;  // トークンの型
  Token* next;     // 次のトークン
  int val;         // kindがTK_NUMの場合、その数値
  char* str;       // トークン文字列
  int len;         // トークンの長さ
};

extern Token* token;
bool at_eof();

bool lookahead(char* op);
bool lookahead_kind(TokenKind kind);
bool consume(char* op);
Token* consume_ident();
void expect(char* op);
int expect_number();
String* expect_string();
Token* tokenize();

typedef struct Type Type;
struct Type {
  enum Ty { INT, CHAR, PTR, ARRAY } ty;
  Type* ptr_to;
  size_t array_size;
};

int get_type_size(Type* type);

// 抽象構文木のノードの種類
typedef enum {
  ND_ADD,       // +
  ND_SUB,       // -
  ND_MUL,       // *
  ND_DIV,       // /
  ND_EQ,        // ==
  ND_NE,        // !=
  ND_LT,        // <
  ND_LE,        // <=
  ND_ASSIGN,    // =
  ND_ADDR,      // &
  ND_DEREF,     // *
  ND_RETURN,    // return
  ND_IF,        // if
  ND_WHILE,     // while
  ND_FOR,       // for
  ND_BLOCK,     // ブロック
  ND_CALL,      // 関数呼び出し
  ND_FUNCTION,  // 関数定義
  ND_LVAR,      // ローカル変数
  ND_LVAR_DEF,  // ローカル変数宣言
  ND_GVAR,      // グローバル変数
  ND_GVAR_DEF,  // グローバル変数定義
  ND_NUM,       // 整数
  ND_STRING,    // 文字列リテラル
  ND_INITS,     // 初期値リスト
} NodeKind;

typedef struct Node Node;
typedef struct NodeVector NodeVector;

// 抽象構文木のノードの型
struct Node {
  NodeKind kind;  // ノードの種類
  Node* lhs;      // 左辺
  Node* rhs;      // 右辺
  int val;        // kindがND_NUMの場合のみ使う
  String* sval;   // kindがND_STRINGの場合のみ使う
  int offset;     // kindがND_LVARの場合のみ使う
  int label;      // kindがND_IF,ND_WHILE,ND_FOR,ND_STRINGの場合のみ使う
  Node* else_stmt;     // kindがND_IFの場合のみ使う
  Node* post_expr;     // kindがND_FORの場合のみ使う
  Node* content_stmt;  // kindがND_FORの場合のみ使う
  NodeVector*
      childs;  // kindがND_BLOCK,ND_CALL,ND_FUNCTION,ND_INITの場合のみ使う
  char* name;    // kindがND_CALLの場合のみ使う
  int name_len;  // kindがND_CALLの場合のみ使う
  Type* type;    // 型
};

struct NodeVector {
  int size;
  int capacity;
  Node** array;
};
NodeVector* new_node_vector();
void add_node(NodeVector* vec, Node* node);

extern Node* code[1000];
void program();

void gen(Node* node);
void gen_strings();

typedef struct GVar GVar;

// グローバル変数の型
struct GVar {
  GVar* next;
  char* name;
  int len;
  Type* type;
  int offset;
};

// グローバル変数
extern GVar* globals;

void gen_globals_list();
