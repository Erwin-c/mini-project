/*
 * tcpepoll.cc, 此程序用于演示采用 Epoll 模型的使用方法.
 *
 *  Author: Erwin
 */

#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/fcntl.h>
#include <sys/socket.h>
#include <sys/types.h> /* See NOTES */
#include <unistd.h>

// 初始化服务端的监听端口.
int initserver(int port);

int main(int argc, char *argv[]) {
  if (argc != 2) {
    printf("Usage: ./tcpepoll port\n");
    return -1;
  }

  // 初始化服务端用于监听的 Socket.
  int listensock = initserver(atoi(argv[1]));
  printf("listensock = %d\n", listensock);

  if (listensock < 0) {
    printf("initserver() failed.\n");
    return -1;
  }

  // 创建 epoll 句柄.
  int epollfd = epoll_create(1);

  // 为监听的 Socket 准备可读事件.
  epoll_event ev;  // 声明事件的数据结构.
  ev.events = EPOLLIN;
  // 指定事件的自定义数据, 会随着 epoll_wait() 返回的事件一并返回.
  ev.data.fd = listensock;

  // 把监听的 Socket 的事件加入 epollfd 中.
  epoll_ctl(epollfd, EPOLL_CTL_ADD, listensock, &ev);

  epoll_event evs[10];  // 存放 epoll 返回的事件.

  while (true) {
    // 等待监视的 Socket 有事件发生.
    int infds = epoll_wait(epollfd, evs, 10, 5000);

    // 返回失败.
    if (infds < 0) {
      perror("epoll() failed.");
      break;
    }

    // 超时.
    if (infds == 0) {
      printf("epoll() timeout.\n");
      continue;
    }

    // 如果 infds > 0, 表示有事件发生的 Socket 的数量.
    // 遍历 epoll 返回的已发生事件的数组 evs.
    for (int ii = 0; ii < infds; ++ii) {
      printf("events = %d, data.fd = %d\n", evs[ii].events, evs[ii].data.fd);

      // 如果发生事件的是 listensock, 表示有新的客户端连上来.
      if (evs[ii].data.fd == listensock) {
        sockaddr_in client;
        socklen_t len = sizeof(client);
        int clientsock = accept(listensock, (sockaddr *)&client, &len);

        printf("accept client (socket = %d) ok.\n", clientsock);

        // 为新客户端准备可读事件, 并添加到 epoll 中.
        ev.data.fd = clientsock;
        ev.events = EPOLLIN;
        epoll_ctl(epollfd, EPOLL_CTL_ADD, clientsock, &ev);
      } else {
        // 如果是客户端连接的 Socket 有事件, 表示有报文发过来或者连接已断开.
        char buffer[1024];  // 存放从客户端读取的数据.
        memset(buffer, 0, sizeof(buffer));
        if (recv(evs[ii].data.fd, buffer, sizeof(buffer), 0) <= 0) {
          // 如果客户端的连接已断开.
          printf("client (eventfd = %d) disconnected.\n", evs[ii].data.fd);
          // 关闭客户端的 Socket.
          close(evs[ii].data.fd);
        } else {
          // 如果客户端有报文发过来.
          printf("recv (eventfd = %d): %s\n", evs[ii].data.fd, buffer);
          // 把接收到的报文内容原封不动的发回去.
          send(evs[ii].data.fd, buffer, strlen(buffer), 0);
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
