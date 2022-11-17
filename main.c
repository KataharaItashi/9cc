#include "9cc.h"

// �������t�@�C���Ƃ��ĊJ����Ȃ�J���Ė߂�l�Ƃ��A�J���Ȃ��Ȃ炻�̂܂ܕԂ�
char *read_file(char *path) {
  // �t�@�C�����J��
  FILE *fp = fopen(path, "r");
  if (!fp)
    return path;
  // �t�@�C���̒����𒲂ׂ�
  if (fseek(fp, 0, SEEK_END) == -1)
    error("%s: fseek: %s", path, strerror(errno));
  size_t size = ftell(fp);
  if (fseek(fp, 0, SEEK_SET) == -1)
    error("%s: fseek: %s", path, strerror(errno));

  // �t�@�C�����e��ǂݍ���
  char *buf = calloc(1, size + 2);
  fread(buf, size, 1, fp);

  // �t�@�C�����K��"\n\0"�ŏI����Ă���悤�ɂ���
  if (size == 0 || buf[size - 1] != '\n')
    buf[size++] = '\n';
  buf[size] = '\0';
  fclose(fp);
  return buf;
}

int main(int argc, char **argv) {
  if (argc != 2){
    error("%s: invalid number of arguments", argv[0]);
    return 1;
  }

  // Tokenize and parse.
  user_input = read_file(argv[1]);
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