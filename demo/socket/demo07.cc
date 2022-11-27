/*
 * demo07.cc, 此程序用于演示采用 TcpClient 类实现 Socket 通讯的客户端.
 *
 *  Author: Erwin
 */

#include "_public.h"

int main(int argc, char *argv[]) {
  if (argc != 3) {
    printf("Using: ./demo07 ip port\nExample: ./demo07 127.0.0.1 5005\n\n");
    return -1;
  }

  CTcpClient TcpClient;

  // 向服务端发起连接请求.
  if (!TcpClient.ConnectToServer(argv[1], atoi(argv[2]))) {
    printf("TcpClient.ConnectToServer(%s, %s) 失败.\n", argv[1], argv[2]);
    return -1;
  }

  char buffer[102400];

  // 与服务端通讯, 发送一个报文后等待回复, 然后再发下一个报文.
  for (int ii = 0; ii < 10; ++ii) {
    SPRINTF(buffer, sizeof(buffer), "这是第 %d 个超级女生, 编号 %03d.", ii + 1,
            ii + 1);

    // 向服务端发送请求报文.
    if (!TcpClient.Write(buffer)) {
      break;
    }

    printf("发送: %s\n", buffer);

    memset(buffer, 0, sizeof(buffer));
    // 接收服务端的回应报文.
    if (!TcpClient.Read(buffer)) {
      break;
    }
    printf("接收: %s\n", buffer);

    // 每隔一秒后再次发送报文.
    sleep(1);
  }

  return 0;
}
