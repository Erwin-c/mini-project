/*
 * demo08.cc, 此程序用于演示采用 TcpServer 类实现 Socket 通讯的服务端.
 *
 *  Author: Erwin
 */

#include "_public.h"

int main(int argc, char *argv[]) {
  if (argc != 2) {
    printf("Using: ./demo08 port\nExample: ./demo08 5005\n\n");
    return -1;
  }

  CTcpServer TcpServer;

  // 服务端初始化.
  if (!TcpServer.InitServer(atoi(argv[1]))) {
    printf("TcpServer.InitServer(%s) 失败.\n", argv[1]);
    return -1;
  }

  // 等待客户端的连接请求.
  if (!TcpServer.Accept()) {
    printf("TcpServer.Accept() 失败.\n");
    return -1;
  }

  printf("客户端 (%s) 已连接.\n", TcpServer.GetIP());

  char buffer[102400];

  // 与客户端通讯, 接收客户端发过来的报文后, 回复 OK.
  while (1) {
    memset(buffer, 0, sizeof(buffer));
    // 接收客户端的请求报文.
    if (!TcpServer.Read(buffer)) {
      break;
    }
    printf("接收: %s\n", buffer);

    strcpy(buffer, "OK");
    // 向客户端发送响应结果.
    if (!TcpServer.Write(buffer)) {
      break;
    }
    printf("发送: %s\n", buffer);
  }

  return 0;
}
