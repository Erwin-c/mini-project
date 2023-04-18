/*
 * reliable_client02.c
 *
 *  Author: Erwin
 */

#include "lib/common.h"

#define MESSAGE_SIZE 102400

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
    rc = send(socket_fd, msg, strlen(msg), 0);
    fprintf(stdout, "send into buffer %ld\n", rc);
    if (rc <= 0) {
      error(1, errno, "send failed");
    }

    --count;
  }

  exit(0);
}
