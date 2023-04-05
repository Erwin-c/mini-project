/*
 * tcp_client.c
 *
 *  Author: Erwin
 */

#include "lib/common.h"

#define MESSAGE_SIZE 102400

void send_data(int sockfd) {
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
    int n_written = send(sockfd, cp, remaining, 0);
    fprintf(stdout, "send into buffer %d\n", n_written);
    if (n_written <= 0) {
      error(1, errno, "send failed");
      return;
    }
    remaining -= n_written;
    cp += n_written;
  }

  free((void*)query);

  return;
}

int main(int argc, char** argv) {
  int sockfd = 0;
  struct sockaddr_in servaddr = {0};

  if (argc != 2) {
    error(1, 0, "usage: tcpclient <IPaddress>");
  }

  sockfd = socket(AF_INET, SOCK_STREAM, 0);

  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(12345);
  inet_pton(AF_INET, argv[1], &servaddr.sin_addr);
  if (connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
    error(1, errno, "connect failed");
  }

  send_data(sockfd);

  exit(0);
}
