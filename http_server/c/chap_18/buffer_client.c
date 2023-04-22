/*
 * buffer_client.c
 *
 *  Author: Erwin
 */

#include "lib/common.h"

int main(int argc, char** argv) {
  int socket_fd = 0;
  size_t buf_len = 0;
  char buf[12] = {0};

  if (argc != 2) {
    error(1, 0, "usage: bufferclient <IPaddress>");
  }

  socket_fd = tcp_client(argv[1], SERV_PORT);

  while (fgets(buf, sizeof(buf), stdin) != NULL) {
    buf_len = strlen(buf);
    if (buf[buf_len - 1] == '\n') {
      buf[buf_len - 1] = '\0';
      --buf_len;
    }

    if (write(socket_fd, buf, buf_len) == -1) {
      error(1, errno, "write failed");
    }
  }

  exit(0);
}
