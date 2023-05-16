/*
 * epoll_server.c
 *
 *  Author: Erwin
 */

#include <sys/epoll.h>

#include "lib/common.h"

#define MAXEVENTS 128

char rot13_char(char c) {
  if ((c >= 'a' && c <= 'm') || (c >= 'A' && c <= 'M')) {
    return c + 13;
  } else if ((c >= 'n' && c <= 'z') || (c >= 'N' && c <= 'Z')) {
    return c - 13;
  } else {
    return c;
  }
}

int main(void) {
  char buf[MAXLINE] = {0};

  int listen_fd = 0;
  listen_fd = tcp_nonblocking_server_listen(SERV_PORT);

  int efd = 0;
  efd = epoll_create1(0);
  if (efd == -1) {
    error(1, errno, "epoll create failed");
  }

  struct epoll_event event = {0};
  event.data.fd = listen_fd;
  event.events = EPOLLIN | EPOLLET;
  if (epoll_ctl(efd, EPOLL_CTL_ADD, listen_fd, &event) == -1) {
    error(1, errno, "epoll_ctl add listen fd failed");
  }

  /* Buffer where events are returned. */
  struct epoll_event *events = NULL;
  events = calloc(MAXEVENTS, sizeof(event));

  while (1) {
    int read_number = 0;
    read_number = epoll_wait(efd, events, MAXEVENTS, -1);

    printf("epoll_wait wakeup\n");

    for (int i = 0; i < read_number; ++i) {
      if ((events[i].events & EPOLLERR) || (events[i].events & EPOLLHUP) ||
          (!(events[i].events & EPOLLIN))) {
        fprintf(stderr, "epoll error\n");

        close(events[i].data.fd);

        continue;
      } else if (listen_fd == events[i].data.fd) {
        struct sockaddr_storage ss = {0};
        socklen_t slen = 0;

        int conn_fd = 0;
        conn_fd = accept(listen_fd, (struct sockaddr *)&ss, &slen);
        if (conn_fd == -1) {
          error(1, errno, "accept failed");
        } else {
          make_nonblocking(conn_fd);

          event.data.fd = conn_fd;
          event.events = EPOLLIN | EPOLLET;  // edge-triggered.
          if (epoll_ctl(efd, EPOLL_CTL_ADD, conn_fd, &event) == -1) {
            error(1, errno, "epoll_ctl add connection fd failed");
          }
        }

        continue;
      } else {
        int socket_fd = 0;
        socket_fd = events[i].data.fd;

        printf("get event on socket fd == %d \n", socket_fd);

        while (1) {
          ssize_t rc = 0;
          if ((rc = read(socket_fd, buf, MAXLINE - 1)) == -1) {
            if (errno != EAGAIN) {
              error(1, errno, "read error");
              close(socket_fd);
            }
            break;
          } else if (rc == 0) {
            close(socket_fd);
            break;
          } else {
            for (int j = 0; j < rc; ++j) {
              buf[j] = rot13_char(buf[j]);
            }

            if (write(socket_fd, buf, rc) == -1) {
              error(1, errno, "write error");
            }
          }
        }
      }
    }
  }

  free(events);

  close(listen_fd);

  exit(0);
}
