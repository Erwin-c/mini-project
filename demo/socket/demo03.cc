/*
 * demo03.cc, 此程序用于演示粘包的 Socket 客户端.
 *
 *  Author: Erwin
 */

#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  if (argc != 3) {
    printf("Using: ./demo03 ip port\nExample: ./demo03 127.0.0.1 5005\n\n");
    return -1;
  }

  // 第 1 步: 创建客户端的 Socket.
  int sockfd;
  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    perror("socket()");
    return -1;
  }

  // 第 2 步: 向服务器发起连接请求.
  hostent *h;
  if ((h = gethostbyname(argv[1])) == nullptr)  // 指定服务端的 IP 地址.
  {
    printf("gethostbyname() 失败.\n");
    close(sockfd);
    return -1;
  }

  sockaddr_in servaddr;
  memset(&servaddr, 0, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(atoi(argv[2]));  // 指定服务端的通讯端口.
  memcpy(&servaddr.sin_addr, h->h_addr, h->h_length);

  // 向服务端发起连接清求.
  if (connect(sockfd, (sockaddr *)&servaddr, sizeof(servaddr)) != 0) {
    perror("connect()");
    close(sockfd);
    return -1;
  }

  int iret;
  char buffer[1024];

  // 第 3 步: 与服务端通讯, 连续发送 1000 个报文.
  for (int ii = 0; ii < 1000; ++ii) {
    memset(buffer, 0, sizeof(buffer));
    sprintf(buffer, "这是第 %d 个超级女生, 编号 %03d.", ii + 1, ii + 1);

    // 向服务端发送请求报文.
    if ((iret = send(sockfd, buffer, strlen(buffer), 0)) <= 0) {
      perror("send()");
      break;
    }

    printf("发送: %s\n", buffer);
  }

  // 第 4 步: 关闭 Socket, 释放资源.
  close(sockfd);

  return 0;
}
