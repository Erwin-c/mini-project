/*
 * grace_server.c
 *
 *  Author: Erwin
 */

#include "lib/common.h"

static int count;

static void sig_int(int signo) {
  printf("signo: %d\n", signo);
  printf("received %d datagrams", count);
  exit(0);
}

int main(void) {
  int listenfd = 0, connfd = 0;
  socklen_t client_len = 0;
  struct sockaddr_in server_addr = {0}, client_addr = {0};
  char message[MAXLINE] = {0}, send_line[MAXLINE + 5] = {0};

  listenfd = socket(AF_INET, SOCK_STREAM, 0);

  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  server_addr.sin_port = htons(SERV_PORT);

  if (bind(listenfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
    error(1, errno, "bind failed");
  }

  if (listen(listenfd, LISTENQ)) {
    error(1, errno, "listen failed");
  }

  signal(SIGINT, sig_int);
  signal(SIGPIPE, SIG_DFL);

  client_len = sizeof(client_addr);
  if ((connfd = accept(listenfd, (struct sockaddr*)&client_addr, &client_len)) <
      0) {
    error(1, errno, "bind failed");
  }

  for (;;) {
    ssize_t read_rc = read(connfd, message, MAXLINE);
    if (read_rc < 0) {
      error(1, errno, "read failed");
    } else if (read_rc == 0) {
      error(1, 0, "client closed");
    }
    message[read_rc] = 0;
    printf("received %ld bytes: %s\n", read_rc, message);
    ++count;

    sprintf(send_line, "Hi, %s", message);

    sleep(5);

    ssize_t write_rc = write(connfd, send_line, strlen(send_line));
    if (write_rc < 0) {
      error(1, errno, "write failed");
    }
    printf("send bytes: %ld\n", write_rc);
  }

  exit(0);
}
