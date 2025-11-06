#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INPUT_CMD_LEN 1024
#define TRUE 1
#define CMD_LEN_MAX 256
#define NUM_BUILTINS 2

char builtins[NUM_BUILTINS][CMD_LEN_MAX] = {"exit", "echo"};

int main(int argc, char *argv[]) {

  while(TRUE) {
    // Flush after every printf
    setbuf(stdout, NULL);

    printf("$ ");

    char input_cmd[INPUT_CMD_LEN];
    fgets(input_cmd, INPUT_CMD_LEN, stdin);

    //remove trailing spaces
    input_cmd[strcspn(input_cmd, "\n")] = '\0';

    if(strncmp(input_cmd, "exit", 4) == 0) {
        exit(0);
    }
    else if(strncmp(input_cmd, "echo", 4) == 0) {
        printf("%s\n", input_cmd+5);
    }
    else if(strncmp(input_cmd, "type", 4) == 0) {
      int i;
      for(i = 0; i < NUM_BUILTINS; i++) {
        if(strncmp(input_cmd+5, builtins[i], strlen(builtins[i])) == 0){
          printf("%s is a shell builtin\n", input_cmd+5);
          break;
        }
      }
      if(i == NUM_BUILTINS)
        printf("%s not found\n", input_cmd+5);
    }
    else
      printf("%s: command not found\n", input_cmd);
  }
  return 0;
}
