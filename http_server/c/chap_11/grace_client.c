/*
 * grace_client.c
 *
 *  Author: Erwin
 */

#include "lib/common.h"

int main(int argc, char** argv) {
  int socket_fd = 0;
  socklen_t server_len = 0;
  struct sockaddr_in server_addr = {0};
  fd_set readmask = {0}, allreads = {0};
  char send_line[MAXLINE] = {0}, recv_line[MAXLINE + 1] = {0};

  if (argc != 2) {
    error(1, 0, "usage: graceclient <IPaddress>");
  }

  socket_fd = socket(AF_INET, SOCK_STREAM, 0);

  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(SERV_PORT);
  inet_pton(AF_INET, argv[1], &server_addr.sin_addr);

  server_len = sizeof(server_addr);
  if (connect(socket_fd, (struct sockaddr*)&server_addr, server_len) < 0) {
    error(1, errno, "connect failed");
  }

  FD_ZERO(&allreads);
  FD_SET(0, &allreads);
  FD_SET(socket_fd, &allreads);
  for (;;) {
    readmask = allreads;
    if (select(socket_fd + 1, &readmask, NULL, NULL, NULL) <= 0) {
      error(1, errno, "select failed");
    }

    if (FD_ISSET(socket_fd, &readmask)) {
      ssize_t n = read(socket_fd, recv_line, MAXLINE);
      if (n < 0) {
        error(1, errno, "read error");
      } else if (n == 0) {
        error(1, 0, "server terminated");
      }
      recv_line[n] = 0;
      fputs(recv_line, stdout);
      fputs("\n", stdout);
    }

    if (FD_ISSET(0, &readmask)) {
      if (fgets(send_line, MAXLINE, stdin) != NULL) {
        if (strncmp(send_line, "shutdown", 8) == 0) {
          FD_CLR(0, &allreads);
          if (shutdown(socket_fd, 1)) {
            error(1, errno, "shutdown failed");
          }
        } else if (strncmp(send_line, "close", 5) == 0) {
          FD_CLR(0, &allreads);
          if (close(socket_fd)) {
            error(1, errno, "close failed");
          }
          sleep(5);
          exit(0);
        } else {
          size_t i = strlen(send_line);
          if (send_line[i - 1] == '\n') {
            send_line[i - 1] = 0;
          }

          printf("now sending %s\n", send_line);
          ssize_t rt = write(socket_fd, send_line, strlen(send_line));
          if (rt < 0) {
            error(1, errno, "write failed");
          }
          printf("send bytes: %ld\n", rt);
        }
      }
    }
  }

  exit(0);
}
