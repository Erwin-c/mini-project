/*
 * stream_server.c
 *
 *  Author: Erwin
 */

#include "lib/common.h"

int count;

char buf[128];

void sig_int(int signo) {
  printf("\nsigno: %d\n", signo);
  printf("\nreceived %d datagrams\n", count);
  exit(0);
}

int main(void) {
  int listen_fd = 0, conn_fd = 0;
  int on = 0;
  ssize_t rc = 0;
  socklen_t client_len = 0;
  struct sockaddr_in server_addr = {0}, client_addr = {0};

  listen_fd = socket(AF_INET, SOCK_STREAM, 0);

  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  server_addr.sin_port = htons(SERV_PORT);

  on = 1;
  setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

  if (bind(listen_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) ==
      -1) {
    error(1, errno, "bind failed");
  }

  if (listen(listen_fd, LISTENQ) == -1) {
    error(1, errno, "listen failed");
  }

  signal(SIGINT, sig_int);
  signal(SIGPIPE, SIG_IGN);

  if ((conn_fd = accept(listen_fd, (struct sockaddr *)&client_addr,
                        &client_len)) == -1) {
    error(1, errno, "accept failed");
  }

  while (1) {
    rc = read_message(conn_fd, buf, sizeof(buf));
    if (rc < 0) {
      error(1, errno, "read message failed");
    } else if (rc == 0) {
      error(1, 0, "client closed");
    }

    buf[rc] = 0;

    printf("received %ld bytes: %s\n", rc, buf);

    ++count;
  }

  exit(0);
}
