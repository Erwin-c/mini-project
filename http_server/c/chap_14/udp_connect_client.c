/*
 * udp_connect_client.c
 *
 *  Author: Erwin
 */

#include "lib/common.h"

char send_line[MAXLINE];
char recv_line[MAXLINE + 1];

int main(int argc, char** argv) {
  int socket_fd = 0;
  struct sockaddr_in server_addr = {0};

  if (argc != 2) {
    error(1, 0, "usage: udpclient <IPaddress>");
  }

  socket_fd = socket(AF_INET, SOCK_DGRAM, 0);

  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(SERV_PORT);
  inet_pton(AF_INET, argv[1], &server_addr.sin_addr);

  if (connect(socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) ==
      -1) {
    error(1, errno, "connect failed");
  }

  while (fgets(send_line, MAXLINE, stdin) != NULL) {
    size_t send_line_len = strlen(send_line);
    if (send_line[send_line_len - 1] == '\n') {
      send_line[send_line_len - 1] = 0;
    }

    printf("now sending %s\n", send_line);

    ssize_t send_rc = send(socket_fd, send_line, strlen(send_line), 0);
    if (send_rc == -1) {
      error(1, errno, "send failed");
    }

    printf("send bytes: %ld\n", send_rc);

    ssize_t recv_rc = recv(socket_fd, recv_line, MAXLINE, 0);
    if (recv_rc == -1) {
      error(1, errno, "recv failed");
    }

    recv_line[recv_rc] = 0;

    fputs(recv_line, stdout);
    fputs("\n", stdout);
  }

  exit(0);
}
