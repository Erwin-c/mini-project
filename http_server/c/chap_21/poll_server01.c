/*
 * poll_server01.c
 *
 *  Author: Erwin
 */

#include "lib/common.h"

#define INIT_SIZE 128

int main(void) {
  char buf[MAXLINE] = {0};

  int listen_fd = 0;
  listen_fd = tcp_server_listen(SERV_PORT);

  // 初始化 pollfd 数组, 这个数组的第一个元素是 listen_fd,
  // 其余的用来记录将要连接的 connect_fd.
  struct pollfd event_set[INIT_SIZE] = {0};
  event_set[0].fd = listen_fd;
  event_set[0].events = POLLRDNORM;

  // 用 -1 表示这个数组位置还没有被占用.
  int i = 0;
  for (i = 1; i < INIT_SIZE; ++i) {
    event_set[i].fd = -1;
  }

  while (1) {
    int ready_number = 0;
    if ((ready_number = poll(event_set, INIT_SIZE, -1)) == -1) {
      error(1, errno, "poll failed");
    }

    if (event_set[0].revents & POLLRDNORM) {
      int connected_fd = 0;
      struct sockaddr_in client_addr = {0};
      socklen_t client_len = 0;
      connected_fd =
          accept(listen_fd, (struct sockaddr*)&client_addr, &client_len);

      // 找到一个可以记录该连接 Socket 的位置.
      for (i = 1; i < INIT_SIZE; ++i) {
        if (event_set[i].fd == -1) {
          event_set[i].fd = connected_fd;
          event_set[i].events = POLLRDNORM;
          break;
        }
      }

      if (i == INIT_SIZE) {
        error(1, errno, "can not hold so many clients");
      }

      if (--ready_number == 0) {
        continue;
      }
    }

    for (i = 1; i < INIT_SIZE; ++i) {
      int socket_fd = 0;
      if ((socket_fd = event_set[i].fd) == -1) {
        continue;
      }

      if (event_set[i].revents & (POLLRDNORM | POLLERR)) {
        ssize_t rc = 0;
        rc = read(socket_fd, buf, MAXLINE - 1);
        if (rc > 0) {
          buf[rc] = '\0';

          printf("now receiving %s\n", buf);

          if (write(socket_fd, buf, rc) == -1) {
            error(1, errno, "write failed");
          }

          printf("send bytes: %zd\n", rc);
        } else if (rc == 0 || errno == ECONNRESET) {
          close(socket_fd);
          event_set[i].fd = -1;
        } else {
          error(1, errno, "read failed");
        }

        if (--ready_number == 0) {
          break;
        }
      }
    }
  }

  exit(0);
}
