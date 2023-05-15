/*
 * server.c
 *
 *  Author: Erwin
 */

#include "lib/common.h"

int main(void) {
  int conn_fd = 0;
  ssize_t rc = 0;
  char buf[1024] = {0};

  conn_fd = tcp_server(SERV_PORT);

  while (1) {
    rc = read(conn_fd, buf, sizeof(buf) - 1);
    if (rc == -1) {
      error(1, errno, "read failed");
    } else if (rc == 0) {
      error(1, 0, "client closed");
    }

    buf[rc] = '\0';

    sleep(5);

    rc = write(conn_fd, buf, rc);
    if (rc == -1) {
      error(1, errno, "write failed");
    }

    printf("send bytes: %zd\n", rc);
  }

  exit(0);
}
