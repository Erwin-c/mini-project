/*
 * udp_server_client.c
 *
 *  Author: Erwin
 */

#include "lib/common.h"

int count;

char message[MAXLINE];
char send_line[MAXLINE + 5];

void recvfrom_int(int signo) {
  printf("\nsigno: %d\n", signo);
  printf("\nreceived %d datagrams\n", count);
  exit(0);
}

int main(void) {
  int socket_fd = 0;
  ssize_t rc = 0;
  socklen_t client_len;
  struct sockaddr_in server_addr = {0}, client_addr = {0};

  socket_fd = socket(AF_INET, SOCK_DGRAM, 0);

  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  server_addr.sin_port = htons(SERV_PORT);

  bind(socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));

  signal(SIGINT, recvfrom_int);

  rc = recvfrom(socket_fd, message, MAXLINE, 0, (struct sockaddr*)&client_addr,
                &client_len);
  if (rc == -1) {
    error(1, errno, "recvfrom failed");
  }

  message[rc] = 0;
  printf("received %ld bytes: %s\n", rc, message);

  if (connect(socket_fd, (struct sockaddr*)&client_addr, client_len) == -1) {
    error(1, errno, "connect failed");
  }

  while (strncmp(message, "goodbye", 7) != 0) {
    sprintf(send_line, "Hi, %s", message);

    rc = send(socket_fd, send_line, strlen(send_line), 0);
    if (rc == -1) {
      error(1, errno, "send failed");
    }
    printf("send bytes: %ld\n", rc);

    rc = recv(socket_fd, message, MAXLINE, 0);
    if (rc == -1) {
      error(1, errno, "recv failed");
    }

    ++count;
  }

  exit(0);
}
