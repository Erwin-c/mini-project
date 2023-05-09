/*
 * buffer_server02.c
 *
 *  Author: Erwin
 */

#include "lib/common.h"

int main(void) {
  int conn_fd = 0;
  ssize_t rc = 0;
  char buf[10] = {0};

  conn_fd = tcp_server(SERV_PORT);

  signal(SIGPIPE, SIG_IGN);

  while (1) {
    rc = read(conn_fd, buf, 9);
    if (rc == -1) {
      error(1, errno, "error read message");
    } else if (rc == 0) {
      error(1, 0, "client closed");
    }

    buf[rc] = '\0';

    printf("received %zd bytes: %s\n", rc, buf);
  }

  exit(0);
}
