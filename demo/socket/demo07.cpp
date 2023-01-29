/*
 * demo07.cpp, 此程序用于演示采用 TcpClient 类实现 Socket 通讯的客户端.
 *
 *  Author: Erwin
 */

import <iostream>;

import <cstdio>;
import <cstring>;

import <unistd.h>;

import idc.network;

int main(int argc, char *argv[]) {
  if (argc != 3) {
    std::cout << "Using: ./demo07 ip port" << std::endl;
    std::cout << "Using: ./demo07 127.0.0.1 5005" << std::endl;
    return -1;
  }

  idc::network::TcpClient client;

  // 向服务端发起连接请求.
  if (!client.connectServer(argv[1], atoi(argv[2]))) {
    std::cout << "client.connectServer(" << argv[1] << ", " << argv[2]
              << ") 失败." << std::endl;
    return -1;
  }

  char buffer[102400] = {0};

  // 与服务端通讯, 发送一个报文后等待回复, 然后再发下一个报文.
  for (int ii = 0; ii < 10; ++ii) {
    snprintf(buffer, sizeof(buffer), "这是第 %d 个超级女生, 编号 %03d.", ii + 1,
             ii + 1);

    // 向服务端发送请求报文.
    if (!client.tcpWrite(buffer)) {
      break;
    }

    std::cout << "发送: " << buffer << std::endl;

    memset(buffer, 0, sizeof(buffer));
    // 接收服务端的回应报文.
    if (!client.tcpRead(buffer)) {
      break;
    }

    std::cout << "接收: " << buffer << std::endl;

    // 每隔一秒后再次发送报文.
    sleep(1);
  }

  return 0;
}
