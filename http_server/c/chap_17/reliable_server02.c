/*
 * reliable_server02.c
 *
 *  Author: Erwin
 */

#include "lib/common.h"

int main(void) {
  int conn_fd = 0;
  int time = 0;
  ssize_t rc = 0;
  char buf[1024] = {0};

  conn_fd = tcp_server(SERV_PORT);

  while (1) {
    rc = read(conn_fd, buf, 1023);
    if (rc == -1) {
      error(1, errno, "read failed");
    } else if (rc == 0) {
      error(1, 0, "client closed");
    }

    buf[rc] = '\0';

    ++time;
    fprintf(stdout, "1K read for %d\n", time);

    usleep(10000);
  }

  exit(0);
}
