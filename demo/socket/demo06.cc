/*
 * demo06.cc, 此程序用于演示不粘包的 Socket 服务端.
 *
 *  Author: Erwin
 */

#include "_public.h"

int main(int argc, char *argv[]) {
  if (argc != 2) {
    printf("Using: ./demo06 port\nExample: ./demo06 5005\n\n");
    return -1;
  }

  // 第 1 步: 创建服务端的 Socket.
  int listenfd;
  if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    perror("socket()");
    return -1;
  }

  // 第 2 步: 把服务端用于通讯的地址和端口绑定到 Socket上.
  sockaddr_in servaddr;  // 服务端地址信息的数据结构.
  memset(&servaddr, 0, sizeof(servaddr));
  servaddr.sin_family = AF_INET;  // 协议族, 在 Socket 编程中只能是 AF_INET.
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);  // 任意 IP 地址.
  servaddr.sin_port = htons(atoi(argv[1]));      // 指定通讯端口.
  if (bind(listenfd, (sockaddr *)&servaddr, sizeof(servaddr)) != 0) {
    perror("bind()");
    close(listenfd);
    return -1;
  }

  // 第3步: 把 Socket 设置为监听模式.
  if (listen(listenfd, 5) != 0) {
    perror("listen()");
    close(listenfd);
    return -1;
  }

  // 第 4 步: 接受客户端的连接.
  int clientfd;                       // 客户端的 Socket.
  int socklen = sizeof(sockaddr_in);  // Sockaddr_in的大小.
  sockaddr_in clientaddr;             // 客户端的地址信息.
  clientfd = accept(listenfd, (sockaddr *)&clientaddr, (socklen_t *)&socklen);
  printf("客户端 (%s) 已连接.\n", inet_ntoa(clientaddr.sin_addr));

  char buffer[1024];

  // 第 5 步: 与客户端通讯, 接收客户端发过来的报文.
  while (1) {
    memset(buffer, 0, sizeof(buffer));

    int ibuflen = 0;
    // 接收客户端的请求报文.
    if (!TcpRead(clientfd, buffer, &ibuflen)) {
      break;
    }

    printf("接收: %s\n", buffer);
  }

  // 第 6 步: 关闭 Socket, 释放资源.
  close(listenfd);
  close(clientfd);

  return 0;
}
