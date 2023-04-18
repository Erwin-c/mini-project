/*
 * reliable_client01.c
 *
 *  Author: Erwin
 */

#include "lib/common.h"

char buf[1024];

int main(void) {
  int conn_fd = 0;
  ssize_t rc = 0;

  conn_fd = tcp_server(SERV_PORT);

  for (;;) {
    rc = read(conn_fd, buf, sizeof(buf));
    if (rc == -1) {
      error(1, errno, "read failed");
    } else if (rc == 0) {
      error(1, 0, "client closed\n");
    }

    buf[rc] = 0;

    sleep(5);

    rc = write(conn_fd, buf, rc);
    if (rc == -1) {
      error(1, errno, "write failed");
    }

    printf("send bytes: %ld\n", rc);
  }

  exit(0);
}
