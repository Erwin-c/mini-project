/*
 * procctl.cc
 *
 *  Author: Erwin
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  if (argc < 3) {
    printf("Using: ./procctl timetvl program argv ...\n");
    printf("Example: ../bin/procctl 5 /usr/bin/tar zcvf ./tmp.tgz ./\n");

    printf("This program is scheduler\n");

    printf("timetvl: time interval\n");
    printf("program: need full path\n");
    printf("argv: arguments\n");

    printf("This program ignores all signals except for SIGKILL\n");

    return -1;
  }

  for (int ii = 0; ii < 64; ++ii) {
    signal(ii, SIG_IGN);
    close(ii);
  }

  // Parent process exits
  // Child process is hosted by systemd and runs in background
  if (fork() != 0) {
    exit(0);
  }

  signal(SIGCHLD, SIG_DFL);

  char *pargv[argc];
  for (int ii = 2; ii < argc; ++ii) {
    pargv[ii - 2] = argv[ii];
  }

  pargv[argc - 2] = NULL;

  while (true) {
    if (fork() == 0) {
      execv(argv[2], pargv);
      exit(0);
    } else {
      int status;
      wait(&status);
      sleep(atoi(argv[1]));
    }
  }

  return 0;
}
