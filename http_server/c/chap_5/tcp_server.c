/*
 * tcp_server.c
 *
 *  Author: Erwin
 */

#include "lib/common.h"

void read_data(int sockfd) {
  int time = 0;
  char buf[1024] = {0};

  for (;;) {
    fprintf(stdout, "block in read\n");
    if (readn(sockfd, buf, 1024) == 0) {
      return;
    }

    ++time;
    fprintf(stdout, "1K read for %d\n", time);
    usleep(1000);
  }

  return;
}

int main(void) {
  int listenfd = 0, connfd = 0;
  socklen_t clilen = 0;
  struct sockaddr_in cliaddr = {0}, servaddr = {0};

  listenfd = socket(AF_INET, SOCK_STREAM, 0);

  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr.sin_port = htons(12345);

  // bind() 到本地地址, 端口为 12345.
  bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr));
  // listen() 的 backlog 为 1024.
  listen(listenfd, 1024);

  // 循环处理用户请求.
  for (;;) {
    clilen = sizeof(cliaddr);
    connfd = accept(listenfd, (struct sockaddr*)&cliaddr, &clilen);
    read_data(connfd);  // 读取数据.
    close(connfd);      // 关闭连接套接字, 注意不是监听套接字.
  }

  exit(0);
}
