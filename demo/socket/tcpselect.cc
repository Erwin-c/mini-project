/*
 * tcpselect.cc, 此程序用于演示采用 Select 模型的使用方法.
 *
 *  Author: Erwin
 */

#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/fcntl.h>
#include <sys/socket.h>
#include <unistd.h>

// 初始化服务端的监听端口.
int initserver(int port);

int main(int argc, char *argv[]) {
  if (argc != 2) {
    printf("usage: ./tcpselect port\n");
    return -1;
  }

  // 初始化服务端用于监听的 Socket.
  int listensock = initserver(atoi(argv[1]));
  printf("listensock = %d\n", listensock);

  if (listensock < 0) {
    printf("initserver() failed.\n");
    return -1;
  }

  // 读事件 Socket 的集合, 包括监听 Socket 和客户端连接上来的 Socket.
  fd_set readfds;
  // 初始化读事件 Socket 的集合.
  FD_ZERO(&readfds);
  // 把 listensock 添加到读事件 Socket 的集合中.
  FD_SET(listensock, &readfds);

  int maxfd = listensock;  // 记录集合中 Socket 的最大值.

  while (true) {
    // 事件: 1) 新客户端的连接请求 accept;
    //      2) 客户端有报文到达 recv, 可以读;
    //      3) 客户端连接已断开;
    //      4) 可以向客户端发送报文 send, 可以写.
    // 可读事件  可写事件
    // select() 等待事件的发生 (监视哪些 Socket 发生了事件).

    fd_set tmpfds = readfds;
    timeval timeout;
    timeout.tv_sec = 10;
    timeout.tv_usec = 0;
    int infds = select(maxfd + 1, &tmpfds, NULL, NULL, &timeout);

    // 返回失败.
    if (infds < 0) {
      perror("select() failed");
      break;
    }

    // 超时, 在本程序中, select() 函数最后一个参数为空, 不存在超时的情况,
    // 但以下代码还是留着.
    if (infds == 0) {
      printf("select() timeout.\n");
      continue;
    }

    // 如果 infds > 0, 表示有事件发生的 Socket 的数量.
    for (int eventfd = 0; eventfd <= maxfd; ++eventfd) {
      // 如果没有事件, continue.
      if (FD_ISSET(eventfd, &tmpfds) <= 0) {
        continue;
      }

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

        // 把新客户端的 Socket 加入可读 Socket 的集合.
        FD_SET(clientsock, &readfds);
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
          // 把已关闭客户端的 Socket 从可读 Socket 的集合中删除.
          FD_CLR(eventfd, &readfds);

          // 重新计算 maxfd 的值, 注意, 只有当 eventfd == maxfd 时才需要计算.
          if (eventfd == maxfd) {
            // 从后面往前找.
            for (int ii = maxfd; ii > 0; --ii) {
              if (FD_ISSET(ii, &readfds)) {
                maxfd = ii;
                break;
              }
            }
          }
        } else {
          // 如果客户端有报文发过来.
          printf("recv (eventfd = %d): %s\n", eventfd, buffer);

          // 把接收到的报文内容原封不动的发回去.
          fd_set tmpfds;
          FD_ZERO(&tmpfds);
          FD_SET(eventfd, &tmpfds);
          if (select(eventfd + 1, NULL, &tmpfds, NULL, NULL) <= 0) {
            perror("select() failed");
          } else {
            send(eventfd, buffer, strlen(buffer), 0);
          }
        }
      }
    }
  }

  return 0;
}

int initserver(int port) {
  int sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock < 0) {
    perror("socket() failed");
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
    perror("bind() failed");
    close(sock);
    return -1;
  }

  if (listen(sock, 5) != 0) {
    perror("listen() failed");
    close(sock);
    return -1;
  }

  return sock;
}
