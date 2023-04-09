/*
 * reliable_client01.c
 *
 *  Author: Erwin
 */

#include "lib/common.h"

char buf[1024];

int main(int argc, char** argv) {
  int socket_fd = 0;

  if (argc != 2) {
    error(1, 0, "usage: reliableclient01 <IPaddress>");
  }

  socket_fd = tcp_client(argv[1], SERV_PORT);

  while (fgets(buf, sizeof(buf), stdin) != NULL) {
    size_t buf_len = strlen(buf);
    if (buf[buf_len - 1] == '\n') {
      buf[buf_len - 1] = 0;
    }

    ssize_t rc = write(socket_fd, buf, buf_len);
    if (rc == -1) {
      error(1, errno, "write failed");
    }

    sleep(3);

    rc = read(socket_fd, buf, sizeof(buf));
    if (rc == -1) {
      // Example: RST, Connection reset by peer.
      error(1, errno, "read failed");
    } else if (rc == 0) {
      error(1, 0, "peer connection closed");
    }

    buf[rc] = 0;

    fputs(buf, stdout);
  }

  exit(0);
}
