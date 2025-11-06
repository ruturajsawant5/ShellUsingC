#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INPUT_CMD_LEN 1024

int main(int argc, char *argv[]) {
  // Flush after every printf
  setbuf(stdout, NULL);

  printf("$ ");

  char input_cmd[INPUT_CMD_LEN];
  fgets(input_cmd, INPUT_CMD_LEN, stdin);

  //remove trailing spaces
  input_cmd[strcspn(input_cmd, "\n")] = '\0';

  printf("%s: command not found\n", input_cmd);

  return 0;
}
