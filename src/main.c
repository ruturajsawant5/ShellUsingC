#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> //access
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

#define INPUT_CMD_LEN 1024
#define TRUE 1
#define CMD_LEN_MAX 256
#define NUM_BUILTINS 3

char builtins[NUM_BUILTINS][CMD_LEN_MAX] = {"exit", "echo", "type"};

int is_shell_builtin(char* cmd)
{
  for(int i = 0; i < NUM_BUILTINS; i++) {
    if(strncmp(cmd, builtins[i], strlen(builtins[i])) == 0){
      return 1;
    }
  }
  return 0;
}

int is_in_path(char* cmd, char* out_full_path)
{
  const char* path_orig = getenv("PATH");
  char *path = strdup(path_orig);

  char* token = strtok(path, ":");
  while(token != NULL)
  {
    char cmd_full_path[1024];
    snprintf(cmd_full_path, 1024, "%s/%s", token, cmd);

    if(access(cmd_full_path, X_OK)==0){
      strcpy(out_full_path, cmd_full_path);
      return 1;
    }
    token = strtok(NULL, ":");
  }

  return 0;
}

int main(int argc, char *argv[]) {

  while(TRUE) {
    // Flush after every printf
    setbuf(stdout, NULL);

    printf("$ ");

    char input_cmd[INPUT_CMD_LEN];
    fgets(input_cmd, INPUT_CMD_LEN, stdin);
    //remove trailing spaces
    input_cmd[strcspn(input_cmd, "\n")] = '\0';

    char *cmd = input_cmd;
    char *args = NULL;
    char *space = strchr(input_cmd, ' ');

    if(space)
    {
      *space = '\0';
      args = space + 1;
    }

    if(strcmp(cmd, "exit") == 0) {
        exit(0);
    }
    else if(strcmp(cmd, "echo") == 0) {
        printf("%s\n", input_cmd+5);
    }
    else if(strcmp(cmd, "type") == 0) {
      int found = 0;

      found = is_shell_builtin(args);
      if(found == 1)
        printf("%s is a shell builtin\n", args);
      else
      {
        char out_full_path[1024];
        found = is_in_path(args, out_full_path);
        if(found == 1)
          printf("%s\n", out_full_path);
        else
          printf("%s not found\n", args);
      }
    }
    else
    {
      char out_full_path[1024];
      int found = is_in_path(cmd, out_full_path);
      if(found == 0){
        printf("%s: command not found\n", cmd);
        continue;
      }

      pid_t pid = fork();
      if (pid < 0) {
          perror("fork");
          return -1;
      }

      if(pid == 0)
      {
        int argc = 0;
        char *argv[32];
        argv[argc++] = cmd; 

        if (args && *args != '\0') {
          char *tok = strtok(args, " ");
          while (tok) {
              argv[argc++] = tok;
              tok = strtok(NULL, " ");
          }
        }

        argv[argc] = NULL;

        execvp(argv[0], argv);
      }
      waitpid(pid, NULL, 0);
    }
  }
  return 0;
}
