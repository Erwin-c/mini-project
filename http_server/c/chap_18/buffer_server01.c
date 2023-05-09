/*
 * buffer_server01.c
 *
 *  Author: Erwin
 */

#include "lib/common.h"

int main(void) {
  int conn_fd = 0;
  ssize_t rc = 0;
  char buffer[128] = {0};
  char response[] = "COMMAND OK";

  conn_fd = tcp_server(SERV_PORT);

  while (1) {
    rc = read(conn_fd, buffer, sizeof(buffer) - 1);
    if (rc == -1) {
      error(1, errno, "read failed");
    } else if (rc == 0) {
      error(1, 0, "client closed");
    }

    // sizeof(buffer), buffer[128] = '\0'.
    buffer[rc] = '\0';

    if (strcmp(buffer, "quit") == 0) {
      printf("client quit\n");
      // sizeof(response), including '\0'.
      write(conn_fd, response, strlen(response));
    }

    printf("received %zd bytes: %s\n", rc, buffer);
  }

  exit(0);
}
