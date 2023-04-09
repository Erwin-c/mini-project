/*
 * reliable_client01.c
 *
 *  Author: Erwin
 */

#include "lib/common.h"

char buf[1024];

int main(void) {
  int conn_fd = 0;

  conn_fd = tcp_server(SERV_PORT);

  for (;;) {
    ssize_t read_rc = read(conn_fd, buf, sizeof(buf));
    if (read_rc < 0) {
      error(1, errno, "error read");
    } else if (read_rc == 0) {
      error(1, 0, "client closed \n");
    }

    buf[read_rc] = 0;

    sleep(5);

    ssize_t write_rc = write(conn_fd, buf, read_rc);
    if (write_rc == -1) {
      error(1, errno, "write failed");
    }

    printf("send bytes: %ld\n", write_rc);
  }

  exit(0);
}
