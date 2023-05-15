/*
 * nonblocking_client.c
 *
 *  Author: Erwin
 */

#include "lib/common.h"

int main(int argc, char** argv) {
  if (argc != 2) {
    error(1, 0, "usage: nonblockingclient <IPaddress>");
  }

  int socket_fd = 0;
  socket_fd = socket(AF_INET, SOCK_STREAM, 0);

  struct sockaddr_in server_addr = {0};
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(SERV_PORT);
  inet_pton(AF_INET, argv[1], &server_addr.sin_addr);

  if (connect(socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) ==
      -1) {
    error(1, errno, "connect failed");
  }

  struct linger ling = {0};
  ling.l_onoff = 1;
  ling.l_linger = 0;
  setsockopt(socket_fd, SOL_SOCKET, SO_LINGER, &ling, sizeof(ling));

  close(socket_fd);

  exit(0);
}
