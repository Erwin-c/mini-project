/*
 * grace_server.c
 *
 *  Author: Erwin
 */

#include "lib/common.h"

int count;

char message[MAXLINE];
char send_line[MAXLINE + 5];

void sig_int(int signo) {
  printf("signo: %d\n", signo);
  printf("received %d datagrams", count);
  exit(0);
}

int main(void) {
  int listen_fd = 0, conn_fd = 0;
  socklen_t client_len = 0;
  struct sockaddr_in server_addr = {0}, client_addr = {0};

  listen_fd = socket(AF_INET, SOCK_STREAM, 0);

  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  server_addr.sin_port = htons(SERV_PORT);

  if (bind(listen_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) <
      0) {
    error(1, errno, "bind failed");
  }

  if (listen(listen_fd, LISTENQ)) {
    error(1, errno, "listen failed");
  }

  signal(SIGINT, sig_int);
  signal(SIGPIPE, SIG_DFL);

  if ((conn_fd = accept(listen_fd, (struct sockaddr*)&client_addr,
                        &client_len)) < 0) {
    error(1, errno, "bind failed");
  }

  for (;;) {
    ssize_t read_rc = read(conn_fd, message, MAXLINE);
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

    ssize_t write_rc = write(conn_fd, send_line, strlen(send_line));
    if (write_rc < 0) {
      error(1, errno, "write failed");
    }

    printf("send bytes: %ld\n", write_rc);
  }

  exit(0);
}
