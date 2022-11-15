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

  // �ϐ�26���̗̈���m�ۂ���
  printf("  push rbp\n");
  printf("  mov rbp, rsp\n");
  printf("  sub rsp, 208\n");

  // �擪�̎����珇�ɃR�[�h����
  for (int i=0; code[i]; i++){
    gen(code[i]);

    // ���̕]�����ʂƂ��ăX�^�b�N�Ɉ�̒l���c���Ă���͂��Ȃ̂ŁA�X�^�b�N�����Ȃ��悤�Ƀ|�b�v���Ă���
    printf("  pop rax\n");
  }

  // �Ō�̎��̌��ʂ�RAX�Ɏc���Ă���̂ł��ꂪ�Ԃ�l�ɂȂ�
  printf("  mov rsp, rbp\n");
  printf("  pop rbp\n");
  printf("  ret\n");
  return 0;
}