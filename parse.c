#include <stdlib.h>
#include <string.h>
#include "tocc.h"

Type* new_type(int ty) {
  Type* type = calloc(1, sizeof(Type));
  type->ty = ty;
  return type;
}

Type* new_ptr_type(Type* ptr_to) {
  Type* type = calloc(1, sizeof(Type));
  type->ty = PTR;
  type->ptr_to = ptr_to;
  return type;
}

Type* new_array_type(Type* ptr_to, size_t size) {
  Type* type = calloc(1, sizeof(Type));
  type->ty = ARRAY;
  type->ptr_to = ptr_to;
  type->array_size = size;
  return type;
}

int get_type_size(Type* type) {
  switch (type->ty) {
    case INT:
      return 4;
    case PTR:
      return 8;
    case ARRAY:
      return type->array_size * get_type_size(type->ptr_to);
    default:
      error("不正なタイプです");
      return 0;
  }
}

typedef struct LVar LVar;

// ローカル変数の型
struct LVar {
  LVar* next;
  char* name;
  int len;
  int offset;
  Type* type;
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

void add_lvar(Token* tok, Type* type) {
  LVar* lvar = find_lvar(tok);
  if (lvar) {
    error_at(tok->str, "変数 %.*s はすでに宣言されています", tok->len,
             tok->str);
  }
  lvar = calloc(1, sizeof(LVar));
  lvar->next = locals;
  lvar->name = tok->str;
  lvar->len = tok->len;
  lvar->type = type;
  int size = get_type_size(type);
  if (locals) {
    lvar->offset = locals->offset + size;
  } else {
    lvar->offset = size;
  }
  locals = lvar;
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
  node->type = new_type(INT);
  return node;
}

Node* new_node_lval(Token* tok) {
  Node* node = calloc(1, sizeof(Node));
  node->kind = ND_LVAR;
  LVar* lvar = find_lvar(tok);
  if (!lvar) {
    error_at(tok->str, "変数 %.*s は宣言されていません", tok->len, tok->str);
  }
  node->offset = lvar->offset;
  node->type = lvar->type;
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
Node* postfix();
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
  expect("int");
  Token* tok = consume_ident();
  if (!tok) {
    error("関数名がありません");
  }
  expect("(");
  Node* node = new_node(ND_FUNCTION, NULL, NULL);
  node->name = tok->str;
  node->name_len = tok->len;
  node->childs = new_node_vector();
  for (;;) {
    if (consume(")")) {
      break;
    }
    if (node->childs->size) {
      expect(",");
    }
    expect("int");
    Token* tok = consume_ident();
    if (!tok) {
      error("関数の引数が不正です");
    }
    add_lvar(tok, new_type(INT));
    add_node(node->childs, new_node_lval(tok));
  }
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
  } else if (consume("int")) {
    // 変数宣言
    Type* type = new_type(INT);
    while (consume("*")) {
      type = new_ptr_type(type);
    }
    Token* name = consume_ident();
    if (consume("[")) {
      type = new_array_type(type, expect_number());
      expect("]");
    }
    add_lvar(name, type);
    expect(";");
    node = stmt();
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
    node->type = node->lhs->type;
  }
  return node;
}

Node* equality() {
  Node* node = relational();

  for (;;) {
    if (consume("==")) {
      node = new_node(ND_EQ, node, relational());
      node->type = new_type(INT);
    } else if (consume("!=")) {
      node = new_node(ND_NE, node, relational());
      node->type = new_type(INT);
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
      node->type = new_type(INT);
    } else if (consume("<=")) {
      node = new_node(ND_LE, node, add());
      node->type = new_type(INT);
    } else if (consume(">")) {
      node = new_node(ND_LT, add(), node);
      node->type = new_type(INT);
    } else if (consume(">=")) {
      node = new_node(ND_LE, add(), node);
      node->type = new_type(INT);
    } else {
      return node;
    }
  }
}

Node* new_add_node(Node* lhs, Node* rhs, NodeKind kind) {
  Type* type = lhs->type;
  if (lhs->type->ty == PTR || lhs->type->ty == ARRAY) {
    // 左辺がポインタの場合は、右辺をprt_toのサイズ倍
    int size = get_type_size(type->ptr_to);
    rhs = new_node(ND_MUL, new_node_num(size), rhs);
  } else if (rhs->type->ty == PTR || lhs->type->ty == ARRAY) {
    // 右辺がポインタの場合は、左辺をprt_toのサイズ倍
    type = rhs->type;
    int size = get_type_size(type->ptr_to);
    lhs = new_node(ND_MUL, new_node_num(size), lhs);
  }
  Node* node = new_node(kind, lhs, rhs);
  node->type = type;
  return node;
}

Node* add() {
  Node* node = mul();

  for (;;) {
    if (consume("+")) {
      Node* rhs = mul();
      node = new_add_node(node, rhs, ND_ADD);
    } else if (consume("-")) {
      Node* rhs = mul();
      node = new_add_node(node, rhs, ND_SUB);
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
      node->type = new_type(INT);
    } else if (consume("/")) {
      node = new_node(ND_DIV, node, unaly());
      node->type = new_type(INT);
    } else {
      return node;
    }
  }
}

Node* unaly() {
  if (consume("+")) {
    return postfix();
  }
  if (consume("-")) {
    Node* node = postfix();
    Type* type = node->type;
    node = new_node(ND_SUB, new_node_num(0), node);
    node->type = type;
    return node;
  }
  if (consume("&")) {
    Node* node = unaly();
    Type* type = new_ptr_type(node->type);
    node = new_node(ND_ADDR, node, NULL);
    node->type = type;
    return node;
  }
  if (consume("*")) {
    Node* node = unaly();
    Type* type = node->type->ptr_to;
    node = new_node(ND_DEREF, node, NULL);
    node->type = type;
    return node;
  }
  if (consume("sizeof")) {
    Node* arg = unaly();
    return new_node_num(get_type_size(arg->type));
  }
  return postfix();
}

Node* postfix() {
  Node* node = primary();
  if (consume("[")) {
    node = new_add_node(node, expr(), ND_ADD);
    Type* type = node->type->ptr_to;
    node = new_node(ND_DEREF, node, NULL);
    node->type = type;
    expect("]");
  }
  return node;
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
      // TODO 関数の本当の返り値の型をtypeに設定する
      node->type = new_type(INT);
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
    return new_node_lval(tok);
  }

  return new_node_num(expect_number());
}
