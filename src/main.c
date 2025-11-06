#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> //access

#define INPUT_CMD_LEN 1024
#define TRUE 1
#define CMD_LEN_MAX 256
#define NUM_BUILTINS 3

char builtins[NUM_BUILTINS][CMD_LEN_MAX] = {"exit", "echo", "type"};

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
      int found = 0;

      for(int i = 0; i < NUM_BUILTINS; i++) {
        if(strncmp(input_cmd+5, builtins[i], strlen(builtins[i])) == 0){
          printf("%s is a shell builtin\n", input_cmd+5);
          found = 1;
        }
      }

      if(found == 0)
      {
        const char* path_orig = getenv("PATH");
        char *path = strdup(path_orig);
        if(!path)
        {
          printf("path not found\n");
          exit(-1);
        }

        char* token = strtok(path, ":");
        while(token != NULL)
        {
          char cmd_full_path[1024];
          snprintf(cmd_full_path, 1024, "%s/%s", token, input_cmd+5);
          //printf("%s\n", cmd_full_path);
          if(access(cmd_full_path, X_OK)==0){
            printf("%s\n", cmd_full_path);
            found = 1;
            break;
          }
          token = strtok(NULL, ":");
        }
        if(found == 0)
          printf("%s not found\n", input_cmd+5);
      }
    }
    else
      printf("%s: command not found\n", input_cmd);
  }
  return 0;
}
