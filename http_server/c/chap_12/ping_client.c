/*
 * ping_client.c
 *
 *  Author: Erwin
 */

#include "lib/common.h"
#include "message_object.h"

#define KEEP_ALIVE_TIME 10
#define KEEP_ALIVE_INTERVAL 3
#define KEEP_ALIVE_PROBETIMES 3

char recv_line[MAXLINE + 1];

int main(int argc, char** argv) {
  int socket_fd = 0;
  int heartbeats = 0;
  struct sockaddr_in server_addr = {0};
  struct timeval tv = {0};
  fd_set readmask = {0}, allreads = {0};
  messageObject message = {0};

  if (argc != 2) {
    error(1, 0, "usage: tcpclient <IPaddress>");
  }

  socket_fd = socket(AF_INET, SOCK_STREAM, 0);

  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(SERV_PORT);
  inet_pton(AF_INET, argv[1], &server_addr.sin_addr);

  if (connect(socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) ==
      -1) {
    error(1, errno, "connect failed");
  }

  tv.tv_sec = KEEP_ALIVE_TIME;
  tv.tv_usec = 0;

  FD_ZERO(&allreads);
  FD_SET(socket_fd, &allreads);
  for (;;) {
    readmask = allreads;

    int select_rc = select(socket_fd + 1, &readmask, NULL, NULL, &tv);
    if (select_rc == -1) {
      error(1, errno, "select failed");
    }

    if (select_rc == 0) {
      if (++heartbeats > KEEP_ALIVE_PROBETIMES) {
        error(1, 0, "connection dead\n");
      }

      printf("sending heartbeat #%d\n", heartbeats);
      message.type = htonl(MSG_PING);
      ssize_t write_rc = write(socket_fd, &message, sizeof(message));
      if (write_rc == -1) {
        error(1, errno, "write failed");
      }

      tv.tv_sec = KEEP_ALIVE_INTERVAL;
      continue;
    }

    if (FD_ISSET(socket_fd, &readmask)) {
      ssize_t read_rc = read(socket_fd, recv_line, MAXLINE);
      if (read_rc == -1) {
        error(1, errno, "read failed");
      } else if (read_rc == 0) {
        error(1, 0, "server terminated\n");
      }

      recv_line[read_rc] = 0;

      printf("received heartbeat, make heartbeats to 0\n");
      heartbeats = 0;
      tv.tv_sec = KEEP_ALIVE_TIME;
    }
  }

  exit(0);
}
