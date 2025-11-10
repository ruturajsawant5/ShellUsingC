#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> //access
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <ctype.h>

#define INPUT_CMD_LEN 1024
#define TRUE 1
#define CMD_LEN_MAX 256
#define NUM_BUILTINS 5
#define MAX_ARGS 32

char builtins[NUM_BUILTINS][CMD_LEN_MAX] = {"exit", "echo", "type", "pwd", "cd"};

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

char** parse_cmd(const char* cmd, int* out_count)
{
    char** args = malloc(MAX_ARGS * sizeof(char*));
    int count = 0;
  
    const char* p = cmd;
    while (*p)
    {
        // skip spaces
        while (isspace((unsigned char)*p))
            p++;
    
        if (*p == '\0')
            break;

        char buffer[1024];
        int bi = 0;

        while (*p && !isspace((unsigned char)*p)) {
            // Handle two single-quotes in a row: skip them
            if (p[0] == '\'' && p[1] == '\'') {
                p += 2; // ignore both
                continue;
            }

            if (p[0] == '\"' && p[1] == '\"') {
                p += 2; // ignore both
                continue;
            }

            if (*p == '\'') {
                // quoted section
                p++; // skip opening '
                while (*p && *p != '\'') {
                    if(p[0] == '\\' && p[1] == '\"')
                      p++;
                    buffer[bi++] = *p++;
                }
                if (*p == '\'')
                    p++; // skip closing '
            } else if (*p == '\"') {
                // quoted section
                p++; // skip opening "
                while (*p && *p != '\"') {
                    if(p[0] == '\\' && ((p[1] == '\'') || (p[1] == '\"') ||  (p[0] == '\\')))
                      p++;
                    buffer[bi++] = *p++;
                }
                if (*p == '\"')
                    p++; // skip closing ""
            } else {
                if(*p == '\\')
                  p++;
                buffer[bi++] = *p++;
            }
        }

        buffer[bi] = '\0';

        if (bi > 0)  // only add if not empty
            args[count++] = strdup(buffer);
    }

    args[count] = NULL;
    *out_count = count;
    return args;
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

    //char *cmd = input_cmd;
    //char *args = NULL;
    //char *space = strchr(input_cmd, ' ');

    //if(space)
    //{
    //  *space = '\0';
    //  args = space + 1;
    //}

    if(input_cmd[0] == 0)
      continue;

    int n_args = 0;
    char** parsed_cmd = parse_cmd(input_cmd, &n_args);

    if(strcmp(parsed_cmd[0], "exit") == 0) {
        exit(0);
    }
    else if(strcmp(parsed_cmd[0], "echo") == 0) {
      for(int i = 1; i < n_args; i++){
        printf("%s", parsed_cmd[i]);
        if(i != n_args - 1)
          printf(" ");
      }
      printf("\n");
    }
    else if(strcmp(parsed_cmd[0], "type") == 0) {
      int found = 0;

      found = is_shell_builtin(parsed_cmd[1]);
      if(found == 1)
        printf("%s is a shell builtin\n", parsed_cmd[1]);
      else
      {
        char out_full_path[1024];
        found = is_in_path(parsed_cmd[1], out_full_path);
        if(found == 1)
          printf("%s\n", out_full_path);
        else
          printf("%s not found\n", parsed_cmd[1]);
      }
    }
    else if(strcmp(parsed_cmd[0], "pwd") == 0) {
      char pwd[1024];
      getcwd(pwd, 1024);
      printf("%s\n",pwd);
    }
    else if(strcmp(parsed_cmd[0], "cd") == 0) {
      const char* path = NULL;
      if(strcmp(parsed_cmd[1],"~")==0)
        path = getenv("HOME");
      else
       path = parsed_cmd[1];
      if(chdir(path) !=0){
        if(errno == ENOENT)
          printf("cd: %s: No such file or directory\n", parsed_cmd[1]);
      }
      

    }
    else
    {
      char out_full_path[1024];
      int found = is_in_path(parsed_cmd[0], out_full_path);
      if(found == 0){
        printf("%s: command not found\n", parsed_cmd[0]);
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
        argv[argc++] = parsed_cmd[0]; 

        //if (args && *args != '\0') {
        //  char *tok = strtok(args, " ");
        //  while (tok) {
        //      argv[argc++] = tok;
        //      tok = strtok(NULL, " ");
        //  }
        //}

        for(int i = 1; i < n_args; i++)
          argv[argc++] = strdup(parsed_cmd[i]);

        argv[argc] = NULL;

        execvp(argv[0], argv);
      }
      waitpid(pid, NULL, 0);
    }
  }
  return 0;
}
