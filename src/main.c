#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INPUT_CMD_LEN 1024
#define TRUE 1
#define CMD_LEN_MAX 256
#define NUM_BUILTINS 1

char builtins[NUM_BUILTINS][CMD_LEN_MAX] = {"exit"};

int main(int argc, char *argv[]) {

  while(TRUE) {
    // Flush after every printf
    setbuf(stdout, NULL);

    printf("$ ");

    char input_cmd[INPUT_CMD_LEN];
    fgets(input_cmd, INPUT_CMD_LEN, stdin);

    //remove trailing spaces
    input_cmd[strcspn(input_cmd, "\n")] = '\0';

    if(strncmp(input_cmd, "exit", 4)) {
        exit(0);
    }

    printf("%s: command not found\n", input_cmd);
  }
  return 0;
}
