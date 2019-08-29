#include <stdlib.h>
#include <string.h>
#include "tocc.h"

typedef struct LVar LVar;

// ローカル変数の型
struct LVar {
  LVar* next;
  char* name;
  int len;
  int offset;
};

// ローカル変数
LVar* locals;

LVar* find_lvar(Token* tok) {
  for (LVar* var = locals; var; var = var->next) {
    if (var->len == tok->len && !strncmp(var->name, tok->str, var->len)) {
      return var;
    }
  }
  return NULL;
}

LVar* new_lvar(Token* tok) {
  LVar* lvar = find_lvar(tok);
  if (lvar) {
    return lvar;
  }
  lvar = calloc(1, sizeof(LVar));
  lvar->next = locals;
  lvar->name = tok->str;
  lvar->len = tok->len;
  if (locals) {
    lvar->offset = locals->offset + 8;
  } else {
    lvar->offset = 8;
  }
  locals = lvar;
  return lvar;
}

Node* new_node(NodeKind kind, Node* lhs, Node* rhs) {
  Node* node = calloc(1, sizeof(Node));
  node->kind = kind;
  node->lhs = lhs;
  node->rhs = rhs;
  return node;
}

Node* new_node_num(int val) {
  Node* node = calloc(1, sizeof(Node));
  node->kind = ND_NUM;
  node->val = val;
  return node;
}

NodeVector* new_node_vector() {
  NodeVector* vec = calloc(1, sizeof(NodeVector));
  vec->capacity = 1;
  vec->array = calloc(1, sizeof(Node*));
  return vec;
}

void add_node(NodeVector* vec, Node* node) {
  if (vec->size == vec->capacity) {
    vec->capacity *= 2;
    vec->array = realloc(vec->array, vec->capacity * sizeof(Node*));
  }
  vec->array[vec->size++] = node;
}

Node* function();
Node* stmt();
Node* expr();
Node* assign();
Node* equality();
Node* relational();
Node* add();
Node* mul();
Node* unaly();
Node* primary();

Node* code[100];

void program() {
  int i = 0;
  while (!at_eof()) {
    code[i++] = function();
  }
  code[i] = NULL;
}

// ラベルの通し番号
int label_number;

Node* block() {
  expect("{");
  Node* node = new_node(ND_BLOCK, NULL, NULL);
  node->childs = new_node_vector();
  while (!consume("}")) {
    add_node(node->childs, stmt());
  }
  return node;
}

Node* function() {
  Token* tok = consume_ident();
  if (!tok) {
    error("関数名がありません");
  }
  expect("(");
  Node* node = new_node(ND_FUNCTION, NULL, NULL);
  node->name = tok->str;
  node->name_len = tok->len;
  node->childs = new_node_vector();
  // todo 引数
  expect(")");
  node->lhs = block();
  return node;
}

Node* stmt() {
  Node* node;

  if (consume("return")) {
    node = new_node(ND_RETURN, expr(), NULL);
    expect(";");
  } else if (consume("if")) {
    expect("(");
    node = new_node(ND_IF, expr(), NULL);
    node->label = label_number++;
    expect(")");
    node->rhs = stmt();
    if (consume("else")) {
      node->else_stmt = stmt();
    }
  } else if (consume("while")) {
    expect("(");
    node = new_node(ND_WHILE, expr(), NULL);
    node->label = label_number++;
    expect(")");
    node->rhs = stmt();
  } else if (consume("for")) {
    expect("(");
    node = new_node(ND_FOR, expr(), NULL);
    node->label = label_number++;
    expect(";");
    node->rhs = expr();
    expect(";");
    node->post_expr = expr();
    expect(")");
    node->content_stmt = stmt();
  } else if (lookahead("{")) {
    node = block();
  } else {
    node = expr();
    expect(";");
  }

  return node;
}

Node* expr() {
  return assign();
}

Node* assign() {
  Node* node = equality();

  if (consume("=")) {
    node = new_node(ND_ASSIGN, node, assign());
  }
  return node;
}

Node* equality() {
  Node* node = relational();

  for (;;) {
    if (consume("==")) {
      node = new_node(ND_EQ, node, relational());
    } else if (consume("!=")) {
      node = new_node(ND_NE, node, relational());
    } else {
      return node;
    }
  }
}

Node* relational() {
  Node* node = add();

  for (;;) {
    if (consume("<")) {
      node = new_node(ND_LT, node, add());
    } else if (consume("<=")) {
      node = new_node(ND_LE, node, add());
    } else if (consume(">")) {
      node = new_node(ND_LT, add(), node);
    } else if (consume(">=")) {
      node = new_node(ND_LE, add(), node);
    } else {
      return node;
    }
  }
}

Node* add() {
  Node* node = mul();

  for (;;) {
    if (consume("+")) {
      node = new_node(ND_ADD, node, mul());
    } else if (consume("-")) {
      node = new_node(ND_SUB, node, mul());
    } else {
      return node;
    }
  }
}

Node* mul() {
  Node* node = unaly();

  for (;;) {
    if (consume("*")) {
      node = new_node(ND_MUL, node, unaly());
    } else if (consume("/")) {
      node = new_node(ND_DIV, node, unaly());
    } else {
      return node;
    }
  }
}

Node* unaly() {
  if (consume("+")) {
    return primary();
  }
  if (consume("-")) {
    return new_node(ND_SUB, new_node_num(0), primary());
  }
  return primary();
}

Node* primary() {
  // 次のトークンが"("なら、"(" expr ")" のはず
  if (consume("(")) {
    Node* node = expr();
    consume(")");
    return node;
  }

  Token* tok = consume_ident();
  if (tok) {
    // 関数呼び出し
    if (consume("(")) {
      Node* node = new_node(ND_CALL, NULL, NULL);
      node->name = tok->str;
      node->name_len = tok->len;
      node->childs = new_node_vector();
      for (;;) {
        if (consume(")")) {
          return node;
        }
        if (node->childs->size) {
          expect(",");
        }
        add_node(node->childs, expr());
      }
    }

    // 左辺値(変数)
    Node* node = calloc(1, sizeof(Node));
    node->kind = ND_LVAR;
    LVar* lvar = new_lvar(tok);
    node->offset = lvar->offset;
    return node;
  }

  return new_node_num(expect_number());
}
