/*
 * udp_client.c
 *
 *  Author: Erwin
 */

#include "lib/common.h"

char send_line[MAXLINE];
char recv_line[MAXLINE + 1];

int main(int argc, char** argv) {
  int socket_fd = 0;
  ssize_t rc = 0;
  size_t send_line_len = 0;
  socklen_t reply_len = 0;
  struct sockaddr_in server_addr = {0}, reply_addr = {0};

  if (argc != 2) {
    error(1, 0, "usage: udpclient <IPaddress>");
  }

  socket_fd = socket(AF_INET, SOCK_DGRAM, 0);

  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(SERV_PORT);
  inet_pton(AF_INET, argv[1], &server_addr.sin_addr);

  /*
  if (connect(socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) ==
      -1) {
    error(1, errno, "connect failed");
  }
  */

  while (fgets(send_line, MAXLINE, stdin) != NULL) {
    send_line_len = strlen(send_line);
    if (send_line[send_line_len - 1] == '\n') {
      send_line[send_line_len - 1] = '\0';
    }

    printf("now sending %s\n", send_line);

    rc = sendto(socket_fd, send_line, strlen(send_line), 0,
                (struct sockaddr*)&server_addr, sizeof(server_addr));
    if (rc < 0) {
      error(1, errno, "sendto failed");
    }

    printf("send bytes: %ld\n", rc);

    recv_line[0] = 0;
    rc = recvfrom(socket_fd, recv_line, MAXLINE, 0,
                  (struct sockaddr*)&reply_addr, &reply_len);
    if (rc < 0) {
      error(1, errno, "recvfrom failed");
    }

    recv_line[rc] = 0;

    fputs(recv_line, stdout);
    fputs("\n", stdout);
  }

  exit(0);
}
