#include <ctype.h>
#include <stdbool.h>

void error(char* fmt, ...);
extern char* user_input;

typedef struct Token Token;
extern Token* token;

bool consume(char* op);
void expect(char* op);
int expect_number();
Token* tokenize(char* p);

// 抽象構文木のノードの種類
typedef enum {
  ND_ADD,  // +
  ND_SUB,  // -
  ND_MUL,  // *
  ND_DIV,  // /
  ND_EQ,   // ==
  ND_NE,   // !=
  ND_LT,   // <
  ND_LE,   // <=
  ND_NUM,  // 整数
} NodeKind;

typedef struct Node Node;

// 抽象構文木のノードの型
struct Node {
  NodeKind kind;  // ノードの種類
  Node* lhs;      // 左辺
  Node* rhs;      // 右辺
  int val;        // kindがND_NUMの場合のみ使う
};

Node* expr();

void gen(Node* node);
