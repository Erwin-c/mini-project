/*
 * demo08.cpp, 此程序用于演示采用 TcpServer 类实现 Socket 通讯的服务端.
 *
 *  Author: Erwin
 */

import <iostream>;

import <cstring>;

import idc.network;

int main(int argc, char *argv[]) {
  if (argc != 2) {
    std::cout << "Using: ./demo08 port" << std::endl;
    std::cout << "Example: ./demo08 5005" << std::endl;
    return -1;
  }

  idc::network::TcpServer server;

  // 服务端初始化.
  if (!server.initServer(atoi(argv[1]))) {
    printf("server.initServer(%s) 失败.\n", argv[1]);
    return -1;
  }

  // 等待客户端的连接请求.
  if (!server.tcpAccept()) {
    printf("server.tcpAccept() 失败.\n");
    return -1;
  }

  std::cout << "客户端 (" << server.getIP() << ") 已连接." << std::endl;

  char buffer[102400];

  // 与客户端通讯, 接收客户端发过来的报文后, 回复 OK.
  while (1) {
    memset(buffer, 0, sizeof(buffer));
    // 接收客户端的请求报文.
    if (!server.tcpRead(buffer)) {
      break;
    }

    std::cout << "接收: " << buffer << std::endl;

    strcpy(buffer, "OK");
    // 向客户端发送响应结果.
    if (!server.tcpWrite(buffer)) {
      break;
    }

    std::cout << "发送: " << buffer << std::endl;
  }

  return 0;
}
