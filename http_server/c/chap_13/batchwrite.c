/*
 * batchwrite.c
 *
 *  Author: Erwin
 */

#include "lib/common.h"

char buf[128];

int main(int argc, char** argv) {
  int socket_fd = 0;
  struct sockaddr_in server_addr = {0};
  struct iovec iov[2] = {0};
  char* send_one = "hello,";

  if (argc != 2) {
    error(1, 0, "usage: batchwrite <IPaddress>");
  }

  socket_fd = socket(AF_INET, SOCK_STREAM, 0);

  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(SERV_PORT);
  inet_pton(AF_INET, argv[1], &server_addr.sin_addr);

  if (connect(socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) <
      0) {
    error(1, errno, "connect failed");
  }

  iov[0].iov_base = send_one;
  iov[0].iov_len = strlen(send_one);
  iov[1].iov_base = buf;
  while (fgets(buf, sizeof(buf), stdin) != NULL) {
    iov[1].iov_len = strlen(buf);
    if (writev(socket_fd, iov, 2) < 0) {
      error(1, errno, "writev failure");
    }
  }

  exit(0);
}
