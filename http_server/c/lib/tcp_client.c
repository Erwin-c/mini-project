/*
 * tcp_client.c
 *
 *  Author: Erwin
 */

#include "common.h"

int tcp_client(char* address, int port) {
  int socket_fd = 0;
  struct sockaddr_in server_addr = {0};

  socket_fd = socket(AF_INET, SOCK_STREAM, 0);

  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(port);
  inet_pton(AF_INET, address, &server_addr.sin_addr);

  if (connect(socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) ==
      -1) {
    error(1, errno, "connect failed");
  }

  return socket_fd;
}
