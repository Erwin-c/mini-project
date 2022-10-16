/*
 * demo04.cc, 此程序用于演示粘包的 Socket 服务端.
 *
 *  Author: Erwin
 */

#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// #include <sys/socket.h>
// #include <sys/types.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  if (argc != 2) {
    printf("Using: ./demo04 port\nExample: ./demo04 5005\n\n");
    return -1;
  }

  // 第 1 步: 创建服务端的 Socket.
  int listenfd;
  if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    perror("socket()");
    return -1;
  }

  // 第 2 步: 把服务端用于通讯的地址和端口绑定到 Socket 上.
  sockaddr_in servaddr;  // 服务端地址信息的数据结构.
  memset(&servaddr, 0, sizeof(servaddr));
  servaddr.sin_family = AF_INET;  // 协议族，在 Socket 编程中只能是 AF_INET.
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);  // 任意 IP 地址.
  servaddr.sin_port = htons(atoi(argv[1]));      // 指定通讯端口.
  if (bind(listenfd, (sockaddr *)&servaddr, sizeof(servaddr)) != 0) {
    perror("bind()");
    close(listenfd);
    return -1;
  }

  // 第 3 步: 把 Socket 设置为监听模式.
  if (listen(listenfd, 5) != 0) {
    perror("listen()");
    close(listenfd);
    return -1;
  }

  // 第 4 步: 接受客户端的连接.
  int clientfd;                       // 客户端的 Socket。
  int socklen = sizeof(sockaddr_in);  // sockaddr_in 的大小
  sockaddr_in clientaddr;             // 客户端的地址信息.
  clientfd = accept(listenfd, (sockaddr *)&clientaddr, (socklen_t *)&socklen);
  printf("Client (%s) has been connected.\n", inet_ntoa(clientaddr.sin_addr));

  int iret;
  char buffer[1024];

  // 第 5 步: 与客户端通讯, 接收客户端发过来的报文.
  while (1) {
    memset(buffer, 0, sizeof(buffer));

    // 接收客户端的请求报文.
    if ((iret = recv(clientfd, buffer, sizeof(buffer), 0)) <= 0) {
      printf("iret = %d\n", iret);
      break;
    }

    printf("Receive: %s\n", buffer);
  }

  // 第 6 步: 关闭 Socket, 释放资源.
  close(listenfd);
  close(clientfd);
}
