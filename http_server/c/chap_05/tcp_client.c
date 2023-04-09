/*
 * tcp_client.c
 *
 *  Author: Erwin
 */

#include "lib/common.h"

#define MESSAGE_SIZE 102400

void send_data(int sock_fd) {
  char* query = NULL;
  const char* cp = NULL;
  size_t remaining = 0;

  query = malloc(MESSAGE_SIZE + 1);
  for (int i = 0; i < MESSAGE_SIZE; ++i) {
    query[i] = 'a';
  }
  query[MESSAGE_SIZE] = '\0';

  cp = query;
  remaining = strlen(query);
  while (remaining != 0) {
    ssize_t write_rc = write(sock_fd, cp, remaining);
    if (write_rc <= 0) {
      error(1, errno, "write failed");
      return;
    }

    fprintf(stdout, "write into buffer %d\n", write_rc);
    remaining -= write_rc;
    cp += write_rc;
  }

  free((void*)query);

  return;
}

int main(int argc, char** argv) {
  int sock_fd = 0;
  struct sockaddr_in server_addr = {0};

  if (argc != 2) {
    error(1, 0, "usage: tcpclient <IPaddress>");
  }

  sock_fd = socket(AF_INET, SOCK_STREAM, 0);

  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(12345);
  inet_pton(AF_INET, argv[1], &server_addr.sin_addr);
  if (connect(sock_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) <
      0) {
    error(1, errno, "connect failed");
  }

  send_data(sock_fd);

  exit(0);
}
