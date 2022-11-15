#include "9cc.h"

int main(int argc, char **argv) {
  if (argc != 2){
    error("%s: invalid number of arguments", argv[0]);
    return 1;
  }

  // Tokenize and parse.
  user_input = argv[1];
  token = tokenize();
  program();

  // Print out the first half of assembly.
  printf(".intel_syntax noprefix\n");
  printf(".global main\n");
  printf("main:\n");

  // 変数26個分の領域を確保する
  printf("  push rbp\n");
  printf("  mov rbp, rsp\n");
  printf("  sub rsp, 208\n");

  // 先頭の式から順にコード生成
  for (int i=0; code[i]; i++){
    gen(code[i]);

    // 式の評価結果としてスタックに一つの値が残っているはずなので、スタックが溢れないようにポップしておく
    printf("  pop rax\n");
  }

  // 最後の式の結果がRAXに残っているのでそれが返り値になる
  printf("  mov rsp, rbp\n");
  printf("  pop rbp\n");
  printf("  ret\n");
  return 0;
}