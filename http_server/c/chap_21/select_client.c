/*
 * select_client.c
 *
 *  Author: Erwin
 */

#include "lib/common.h"

int main(int argc, char** argv) {
  char recv_line[MAXLINE] = {0}, send_line[MAXLINE] = {0};

  if (argc != 2) {
    error(1, 0, "usage: select <IPaddress>");
  }

  int socket_fd = 0;
  socket_fd = tcp_client(argv[1], SERV_PORT);

  fd_set readmask = {0}, allreads = {0};
  FD_SET(0, &allreads);
  FD_SET(socket_fd, &allreads);

  while (1) {
    readmask = allreads;

    if (select(socket_fd + 1, &readmask, NULL, NULL, NULL) == -1) {
      error(1, errno, "select failed");
    }

    ssize_t rc = 0;

    if (FD_ISSET(socket_fd, &readmask)) {
      rc = read(socket_fd, recv_line, MAXLINE - 1);
      if (rc == -1) {
        error(1, errno, "read error");
      } else if (rc == 0) {
        error(1, 0, "server terminated");
      }

      recv_line[rc] = '\0';
      fputs(recv_line, stdout);
      fputs("\n", stdout);
    }

    if (FD_ISSET(STDIN_FILENO, &readmask)) {
      // readmask[socket_fd] = 0.
      if (fgets(send_line, MAXLINE, stdin) != NULL) {
        size_t buf_len = 0;
        buf_len = strlen(send_line);
        if (send_line[buf_len - 1] == '\n') {
          send_line[buf_len - 1] = '\0';
          --buf_len;
        }

        printf("now sending %s\n", send_line);

        rc = write(socket_fd, send_line, buf_len);
        if (rc == -1) {
          error(1, errno, "write failed");
        }

        printf("send bytes: %zd\n", rc);
      }
    }
  }

  exit(0);
}
