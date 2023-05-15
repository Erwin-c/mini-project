/*
 * tcp_server.c
 *
 *  Author: Erwin
 */

#include "common.h"

int tcp_server(int port) {
  int listen_fd = 0;
  listen_fd = socket(AF_INET, SOCK_STREAM, 0);

  int on = 1;
  setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

  struct sockaddr_in server_addr = {0};
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  server_addr.sin_port = htons(port);
  if (bind(listen_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) ==
      -1) {
    error(1, errno, "bind failed");
  }

  if (listen(listen_fd, LISTENQ) == -1) {
    error(1, errno, "listen failed");
  }

  signal(SIGPIPE, SIG_IGN);

  int conn_fd = 0;
  struct sockaddr_in client_addr = {0};
  socklen_t client_len = 0;
  if ((conn_fd = accept(listen_fd, (struct sockaddr *)&client_addr,
                        &client_len)) == -1) {
    error(1, errno, "accept failed");
  }

  return conn_fd;
}

int tcp_server_listen(int port) {
  int listen_fd;
  listen_fd = socket(AF_INET, SOCK_STREAM, 0);

  int on = 1;
  setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

  struct sockaddr_in server_addr = {0};
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  server_addr.sin_port = htons(port);

  if (bind(listen_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) ==
      -1) {
    error(1, errno, "bind failed");
  }

  if (listen(listen_fd, LISTENQ) == -1) {
    error(1, errno, "listen failed");
  }

  signal(SIGPIPE, SIG_IGN);

  return listen_fd;
}

int tcp_nonblocking_server_listen(int port) {
  int listenfd = 0;
  listenfd = socket(AF_INET, SOCK_STREAM, 0);

  make_nonblocking(listenfd);

  struct sockaddr_in server_addr = {0};
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  server_addr.sin_port = htons(port);

  int on = 1;
  setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

  if (bind(listenfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) ==
      -1) {
    error(1, errno, "bind failed");
  }

  if (listen(listenfd, LISTENQ) == -1) {
    error(1, errno, "listen failed");
  }

  signal(SIGPIPE, SIG_IGN);

  return listenfd;
}

void make_nonblocking(int fd) {
  fcntl(fd, F_SETFL, O_NONBLOCK);
  return;
}
