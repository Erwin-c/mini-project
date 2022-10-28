/*
 * tcppoll.cc, 此程序用于演示采用 poll 模型的使用方法.
 *
 *  Author: Erwin
 */

#include <arpa/inet.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/fcntl.h>
#include <sys/socket.h>
#include <unistd.h>

// ulimit -a
#define MAXNFDS 1024

// 初始化服务端的监听端口.
int initserver(int port);

int main(int argc, char *argv[]) {
  if (argc != 2) {
    printf("Usage: ./tcppoll port\n");
    return -1;
  }

  // 初始化服务端用于监听的 Socket.
  int listensock = initserver(atoi(argv[1]));
  printf("listensock = %d\n", listensock);

  if (listensock < 0) {
    printf("initserver() failed.\n");
    return -1;
  }

  struct pollfd fds[MAXNFDS];  // fds 存放需要监视的 Socket.
  for (int ii = 0; ii < MAXNFDS; ++ii) {
    // 初始化数组, 把全部的 fd 设置为 -1.
    fds[ii].fd = -1;
  }

  // 把 listensock 和读事件添加到数组中.
  fds[listensock].fd = listensock;
  fds[listensock].events = POLLIN;

  int maxfd = listensock;  // fds 数组中需要监视的 Socket 的大小.

  while (true) {
    int infds = poll(fds, maxfd + 1, 5000);

    // 返回失败.
    if (infds < 0) {
      perror("poll() failed.");
      break;
    }

    // 超时.
    if (infds == 0) {
      printf("poll() timeout.\n");
      continue;
    }

    // 如果 infds > 0, 表示有事件发生的 Socket 的数量.
    // 这里是客户端的 Socket事件, 每次都要遍历整个数组,
    // 因为可能有多个 Socket 有事件.
    for (int eventfd = 0; eventfd <= maxfd; ++eventfd) {
      if (fds[eventfd].fd < 0) {
        // 如果 fd 为负, 忽略它.
        continue;
      }

      if ((fds[eventfd].revents & POLLIN) == 0) {
        // 如果没有事件, continue.
        continue;
      }

      // 先把 revents 清空.
      fds[eventfd].revents = 0;

      // 如果发生事件的是 listensock, 表示有新的客户端连上来.
      if (eventfd == listensock) {
        sockaddr_in client;
        socklen_t len = sizeof(client);
        int clientsock = accept(listensock, (sockaddr *)&client, &len);
        if (clientsock < 0) {
          perror("accept() failed.");
          continue;
        }

        printf("accept client (socket = %d) ok.\n", clientsock);

        // 修改 fds 数组中 clientsock 的位置的元素.
        fds[clientsock].fd = clientsock;
        fds[clientsock].events = POLLIN;
        fds[clientsock].revents = 0;
        if (maxfd < clientsock) {
          // 更新 maxfd 的值.
          maxfd = clientsock;
        }
      } else {
        // 如果是客户端连接的 Socket 有事件, 表示有报文发过来或者连接已断开.

        char buffer[1024];  // 存放从客户端读取的数据.
        memset(buffer, 0, sizeof(buffer));
        if (recv(eventfd, buffer, sizeof(buffer), 0) <= 0) {
          // 如果客户端的连接已断开.
          printf("client (eventfd = %d) disconnected.\n", eventfd);
          // 关闭客户端的 Socket.
          close(eventfd);
          fds[eventfd].fd = -1;

          // 重新计算 maxfd 的值, 注意, 只有当 eventfd == maxfd 时才需要计算.
          if (eventfd == maxfd) {
            // 从后面往前找.
            for (int ii = maxfd; ii > 0; --ii) {
              if (fds[ii].fd != -1) {
                maxfd = ii;
                break;
              }
            }
          }
        } else {
          // 如果客户端有报文发过来.
          printf("recv (eventfd = %d): %s\n", eventfd, buffer);
          send(eventfd, buffer, strlen(buffer), 0);
        }
      }
    }
  }

  return 0;
}

int initserver(int port) {
  int sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock < 0) {
    perror("socket() failed.");
    return -1;
  }

  int opt = 1;
  unsigned int len = sizeof(opt);
  setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, len);

  sockaddr_in servaddr;
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr.sin_port = htons(port);

  if (bind(sock, (sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
    perror("bind() failed.");
    close(sock);
    return -1;
  }

  if (listen(sock, 5) != 0) {
    perror("listen() failed.");
    close(sock);
    return -1;
  }

  return sock;
}
