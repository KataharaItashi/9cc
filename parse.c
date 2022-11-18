#include "9cc.h"

//
// Tokenizer
//

// Input program
char *user_input;

// Current token
Token *token;

// Reports an error and exit.
void error(char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

// Reports an error location and exit.
void error_at(char *loc, char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);

  int pos = loc - user_input;
  fprintf(stderr, "%s\n", user_input);
  fprintf(stderr, "%*s", pos, ""); // print pos spaces.
  fprintf(stderr, "^ ");
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

// Consumes the current token if it matches `op`.
bool consume(char *op) {
  if (token->kind != TK_RESERVED || strlen(op) != token->len || memcmp(token->str, op, token->len) )
    return false;
  token = token->next;
  return true;
}

// consumes the current token if it matches kind
bool consume_kind(TokenKind kind){
  if (token->kind != kind)
    return false;
  token = token->next;
  return true;
}

// Ensure that the current token is `op`.
void expect(char *op) {
  if (token->kind != TK_RESERVED || strlen(op) != token->len || memcmp(token->str, op, token->len) )
    error_at(token->str, "expected '%c'", op);
  token = token->next;
}

// Ensure that the current token is TK_NUM.
int expect_number() {
  if (token->kind != TK_NUM)
    error_at(token->str, "expected a number");
  int val = token->val;
  token = token->next;
  return val;
}

Token *consume_ident(){
  if (token->kind != TK_IDENT)
    return NULL;
  Token *tok = token;
  token = token->next;
  return tok;
}

bool at_eof() {
  return token->kind == TK_EOF;
}

// Create a new token and add it as the next token of `cur`.
Token *new_token(TokenKind kind, Token *cur, char *str, int len) {
  Token *tok = calloc(1, sizeof(Token));
  tok->kind = kind;
  tok->str = str;
  tok->len = len;
  cur->next = tok;
  return tok;
}

bool startswith(char *p, char *q){
  return memcmp(p, q, strlen(q)) == 0;
}

// 文字がアルファベットまたは数字か？
int is_alnum(char c){
  return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || ('0' <= c && c <= '9') || (c == '_');
}

// Tokenize `user_input` and returns new tokens.
Token *tokenize() {
  char *p = user_input;
  Token head;
  head.next = NULL;
  Token *cur = &head;

  while (*p) {
    // Skip whitespace characters.
    if (isspace(*p)) {
      p++;
      continue;
    }

    // Multi-letter punctuator
    if (startswith(p, "==") || startswith(p, "!=") || startswith(p, "<=") || startswith(p, ">=")){
      cur = new_token(TK_RESERVED, cur, p, 2);
      p += 2;
      continue;
    }

    // Single-letter punctuator
    if (strchr("+-*/()<>=;", *p)) {
      cur = new_token(TK_RESERVED, cur, p++, 1);
      continue;
    }

    // Integer literal
    if (isdigit(*p)) {
      cur = new_token(TK_NUM, cur, p, 0);
      char *q = p;
      cur->val = strtol(p, &p, 10);
      cur->len = p - q;
      continue;
    }

    // return文
    if (startswith(p, "return") && !is_alnum(p[6])){
      cur = new_token(TK_RETURN, cur, p, 6);
      p += 6;
      continue;
    }

    // if文
    if (startswith(p, "if") && !is_alnum(p[2])){
      cur = new_token(TK_IF, cur, p, 2);
      p +=2;
      continue;
    }

    // for文
    if (startswith(p, "for") && !is_alnum(p[3])){
      cur = new_token(TK_FOR, cur, p, 3);
      p += 3;
      continue;
    }

    // 識別子
    if ('a' <= *p && *p <= 'z') {
      cur = new_token(TK_IDENT, cur, p++, 1);
      continue;
    }

    error_at(p, "invalid token");
  }

  new_token(TK_EOF, cur, p, 0);
  return head.next;
}

//
// Parser
//

// コードの配列（上限100）
Node *code[100];

Node *new_node(NodeKind kind) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = kind;
  return node;
}

Node *new_binary(NodeKind kind, Node *lhs, Node *rhs) {
  Node *node = new_node(kind);
  node->lhs = lhs;
  node->rhs = rhs;
  return node;
}

Node *new_num(int val) {
  Node *node = new_node(ND_NUM);
  node->val = val;
  return node;
}

void program() {
  int i = 0;
  while (!at_eof())
    code[i++] = stmt();
  code[i] = NULL;
}

Node *stmt() {
  Node *node;

  if (consume_kind(TK_IF)){
    expect("(");
    node = calloc(1, sizeof(Node));
    node->kind = ND_IF;
    node->lhs = expr();
    expect(")");
    node->rhs = stmt();
    return node;
  }

  if(consume_kind(TK_FOR)){
    expect("(");
    node = calloc(1, sizeof(Node));
    node->kind = ND_FOR;

    Node *left = calloc(1,sizeof(Node));
    left->kind = ND_FOR_LEFT;
    Node *right = calloc(1,sizeof(Node));
    right->kind = ND_FOR_RIGHT;

    if(!consume(";")){
    left->lhs = stmt();
    }

    if(!consume(";")){
    left->rhs = expr();
    expect(";");
    }

    if(!consume(")")){
    right->lhs = expr();
    expect(")");
    }

    right->rhs = stmt();
    
    node->lhs = left;
    node->rhs = right;
    return node;
  } 

  if (consume_kind(TK_RETURN)){
    node = calloc(1, sizeof(Node));
    node->kind = ND_RETURN;
    node->lhs = expr();
  } else {
    node = expr();
  }

  if(!consume(";"))
    error_at(token->str, "';'ではないトークンです");
  return node;
}

Node *expr() {
  return assign();
}

Node *assign() {
  Node *node = equality();
  if (consume("="))
    node = new_binary(ND_ASSIGN, node ,assign());
  return node;
}

Node *equality(){
  Node *node = relational();

  for (;;){
    if (consume("=="))
      node = new_binary(ND_EQ, node, relational());
    else if (consume("!="))
      node = new_binary(ND_NE, node, relational());
    else 
      return node;
  }
}

Node *relational(){
  Node *node = add();

  for(;;){
    if (consume("<"))
      node = new_binary(ND_LT, node, add());
    else if (consume("<="))
      node = new_binary(ND_LE, node, add());
    else if (consume(">"))
      node = new_binary(ND_LT, add(), node);
    else if (consume(">="))
      node = new_binary(ND_LE, add(), node);
    else
      return node;
  }
}

Node *add(){
  Node *node = mul();

  for (;;) {
  if (consume("+"))
    node = new_binary(ND_ADD, node, mul());
  else if (consume("-"))
    node = new_binary(ND_SUB, node, mul());
  else
    return node;
  }
}

Node *mul() {
  Node *node = primary();

  for (;;) {
    if (consume("*"))
      node = new_binary(ND_MUL, node, primary());
    else if (consume("/"))
      node = new_binary(ND_DIV, node, primary());
    else
      return node;
  }
}

Node *primary() {
  if (consume("(")) {
    Node *node = expr();
    expect(")");
    return node;
  }

  Token *tok = consume_ident();
  if (tok) {
    Node *node = calloc(1, sizeof(Node));
    node->kind = ND_LVAR;
    node->offset = (tok->str[0] - 'a' +1 ) * 8;
    return node;
  }

  return new_num(expect_number());
}