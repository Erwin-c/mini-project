/*
 * tcp_server.c
 *
 *  Author: Erwin
 */

#include "lib/common.h"

char buf[1024];

void read_data(int sockfd) {
  int time = 0;

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
  int listen_fd = 0, conn_fd = 0;
  socklen_t client_len = 0;
  struct sockaddr_in client_addr = {0}, server_addr = {0};

  listen_fd = socket(AF_INET, SOCK_STREAM, 0);

  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  server_addr.sin_port = htons(12345);

  // bind() 到本地地址, 端口为 12345.
  bind(listen_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));

  // listen() 的 backlog 为 1024.
  listen(listen_fd, 1024);

  // 循环处理用户请求.
  for (;;) {
    conn_fd = accept(listen_fd, (struct sockaddr*)&client_addr, &client_len);
    memset(buf, 0, sizeof(buf));
    read_data(conn_fd);  // 读取数据.
    close(conn_fd);      // 关闭连接套接字, 注意不是监听套接字.
  }

  exit(0);
}
