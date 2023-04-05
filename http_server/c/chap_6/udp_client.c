/*
 * udp_client.c
 *
 *  Author: Erwin
 */

#include "lib/common.h"

#define NDG 2000    // datagrams to send.
#define DGLEN 1400  // length of each datagram.
#define MAXLINE 4096

int main(int argc, char** argv) {
  int socket_fd = 0;
  socklen_t server_len = 0, reply_len = 0;
  struct sockaddr_in server_addr = {0}, reply_addr = {0};
  char send_line[MAXLINE] = {0}, recv_line[MAXLINE + 1] = {0};

  if (argc != 2) {
    error(1, 0, "usage: udpclient <IPaddress>");
  }

  socket_fd = socket(AF_INET, SOCK_DGRAM, 0);

  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(SERV_PORT);
  inet_pton(AF_INET, argv[1], &server_addr.sin_addr);

  server_len = sizeof(server_addr);

  socklen_t len;
  int n;

  while (fgets(send_line, MAXLINE, stdin) != NULL) {
    int i = strlen(send_line);
    if (send_line[i - 1] == '\n') {
      send_line[i - 1] = 0;
    }

    printf("now sending %s\n", send_line);
    ssize_t rt = sendto(socket_fd, send_line, strlen(send_line), 0,
                        (struct sockaddr*)&server_addr, server_len);
    if (rt < 0) {
      error(1, errno, "sendto failed");
    }
    printf("send bytes: %lu\n", rt);

    len = 0;
    n = recvfrom(socket_fd, recv_line, MAXLINE, 0, reply_addr, &len);
    if (n < 0) {
      error(1, errno, "recvfrom failed");
    }
    recv_line[n] = 0;
    fputs(recv_line, stdout);
    fputs("\n", stdout);
  }

  exit(0);
}
