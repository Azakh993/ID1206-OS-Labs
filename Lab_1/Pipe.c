#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

/**
 * Creates a pipe and checks for errors.
 * @param pipefd An array to store the file descriptors of the pipe.
 */
void createPipeAndCheck(int pipefd[2]);

/**
 * Configures the child process to write to the pipe.
 * @param pipefd An array with file descriptors of the pipe.
 */
void configureChildProcess(int pipefd[2]);

/**
 * Executes the command in the child process.
 */
void executeChildCommand();

/**
 * Configures the parent process to read from the pipe.
 * @param pipefd An array with file descriptors of the pipe.
 */
void configureParentProcess(int pipefd[2]);

/**
 * Executes the command in the parent process.
 */
void executeParentCommand();

int main() {
  int pipefd[2];
  createPipeAndCheck(pipefd);

  pid_t pid = fork();

  switch (pid) {
    case -1:
      perror("fork");
      exit(EXIT_FAILURE);

    case 0:
      configureChildProcess(pipefd);
      executeChildCommand();
      break;

    default:
      configureParentProcess(pipefd);
      executeParentCommand();
      break;
  }

  return 0;
}

void createPipeAndCheck(int pipefd[2]) {
  int pipeCreationState = pipe(pipefd);

  if (pipeCreationState == -1) {
    perror("pipe");
    exit(EXIT_FAILURE);
  }
}

void configureChildProcess(int pipefd[2]) {
  close(pipefd[0]);
  dup2(pipefd[1], STDOUT_FILENO);
  close(pipefd[1]);
}

void executeChildCommand() {
  execlp("ls", "ls", "/", NULL);

  perror("execlp");
  exit(EXIT_FAILURE);
}

void configureParentProcess(int pipefd[2]) {
  close(pipefd[1]);
  dup2(pipefd[0], STDIN_FILENO);
  close(pipefd[0]);
}

void executeParentCommand() {
  wait(NULL);
  execlp("wc", "wc", "-l", NULL);

  perror("execlp");
  exit(EXIT_FAILURE);
}
