#include "ssiv.h"
#include <errno.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/wait.h>

int is_binary_available_posix(const char *binary_name) {
  pid_t pid = fork();
  if (pid == -1) {
    return 0;
  }

  if (pid == 0) {
    freopen("/dev/null", "w", stdout);
    freopen("/dev/null", "w", stderr);

    char *args[] = {(char *)binary_name, "--version", NULL};
    execvp(binary_name, args);

    // If execvp returns, it failed. ENOENT means "file not found".
    _exit(errno == ENOENT ? 127 : 126);
  }

  int status;
  waitpid(pid, &status, 0);

  // If the child exited normally and returned 127, the binary wasn't found
  if (WIFEXITED(status) && WEXITSTATUS(status) == 127) {
    return 0; // Not available
  }

  return 1;
}

int feh_init(void) { return is_binary_available_posix("feh"); }

int feh_set_wallpaper(const char *image_path) {
  // TODO: Make forks wait for other wallpaper setting forks
  pid_t pid = fork();
  if (pid == -1) {
    return 0;
  }

  if (pid == 0) {
    // printf("sxiv: setting...\n");

    char *args[] = {(char *)"feh", "--bg-fill", (char *)image_path, NULL};
    printf("args: %s %s\n", args[0], args[1]);
    execvp("feh", args);

    // TODO: Just because a backend fails we shouldn't kill the app
    // If execvp returns, it failed. ENOENT means "file not found".
    _exit(errno == ENOENT ? 127 : 126);
  }

  int status;
  waitpid(pid, &status, 0);
  // printf("sxiv: done setting.\n");

  return 0;
}

void feh_cleanup(void) {}

int arg_action(const char *image_path) {
  // TODO: Make forks wait for other wallpaper setting forks
  pid_t pid = fork();
  if (pid == -1) {
    return 0;
  }

  if (pid == 0) {
    // printf("ssiv: setting...\n");
    char *name = strdup(arg_backend.name);
    char *token = strtok(name, " ");
    char *executable = strdup(token);
    char **args = malloc(2 * sizeof(char *));
    int args_size = 0;
    int args_capacity = 2;
    args[0] = executable;
    args_size++;
    while (token != NULL) {
      token = strtok(NULL, " ");

      if (args_size >= args_capacity) {
        args_capacity = args_capacity * 2;
        args = realloc(args, sizeof(char *) * args_capacity);
      }

      args[args_size] = token;
      args_size++;
    }

    args_capacity += 2;
    args = realloc(args, sizeof(char *) * args_capacity);
    args[--args_size] = (char *)image_path;
    args[++args_size] = NULL;

    execvp(executable, args);
    free(executable);
    free(name);

    // TODO: Just because a backend fails we shouldn't kill the app
    // If execvp returns, it failed. ENOENT means "file not found".
    _exit(errno == ENOENT ? 127 : 126);
  }

  int status;
  waitpid(pid, &status, 0);

  return 0;
}
