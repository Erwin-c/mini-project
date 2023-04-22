/*
 * reliable_client02.c
 *
 *  Author: Erwin
 */

#include "lib/common.h"

int main(int argc, char** argv) {
  char* msg = "network programming";
  int count = 10000000;
  int socket_fd = 0;
  ssize_t rc = 0;

  if (argc != 2) {
    error(1, 0, "usage: reliableclient02 <IPaddress>");
  }

  socket_fd = tcp_client(argv[1], SERV_PORT);

  signal(SIGPIPE, SIG_IGN);

  while (count > 0) {
    rc = write(socket_fd, msg, strlen(msg));
    fprintf(stdout, "write into buffer %ld\n", rc);
    if (rc <= 0) {
      error(1, errno, "write failed");
    }

    --count;
  }

  exit(0);
}
