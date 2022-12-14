/*
 * demo13.cc, 此程序用于演示网银 APP 软件的客户端, 增加了心跳报文.
 *
 *  Author: Erwin
 */

#include "_public.h"

CTcpClient TcpClient;

// 心跳.
bool srv000();

// 登录业务
bool srv001();

// 我的账户 (查询余额).
bool srv002();

int main(int argc, char *argv[]) {
  if (argc != 3) {
    printf("Using: ./demo13 ip port\nExample: ./demo13 127.0.0.1 5005\n\n");
    return -1;
  }

  // 向服务端发起连接请求.
  if (!TcpClient.ConnectToServer(argv[1], atoi(argv[2]))) {
    printf("TcpClient.ConnectToServer(%s, %s) 失败.\n", argv[1], argv[2]);
    return -1;
  }

  // 登录业务.
  if (!srv001()) {
    printf("srv001() 失败.\n");
    return -1;
  }

  sleep(3);

  // 我的账户 (查询余额).
  if (!srv002()) {
    printf("srv002() 失败.\n");
    return -1;
  }

  sleep(10);

  for (int ii = 3; ii < 5; ++ii) {
    if (!srv000()) {
      break;
    }

    sleep(ii);
  }

  // 我的账户 (查询余额).
  if (!srv002()) {
    printf("srv002() 失败.\n");
    return -1;
  }

  return 0;
}

bool srv000() {
  char buffer[1024];

  SPRINTF(buffer, sizeof(buffer), "<srvcode>0</srvcode>");
  printf("发送: %s\n", buffer);
  // 向服务端发送请求报文.
  if (!TcpClient.Write(buffer)) {
    return false;
  }

  memset(buffer, 0, sizeof(buffer));
  // 接收服务端的回应报文.
  if (!TcpClient.Read(buffer)) {
    return false;
  }
  printf("接收: %s\n", buffer);

  return true;
}

bool srv001() {
  char buffer[1024];

  SPRINTF(
      buffer, sizeof(buffer),
      "<srvcode>1</srvcode><tel>1392220000</tel><password>123456</password>");
  printf("发送: %s\n", buffer);
  // 向服务端发送请求报文.
  if (!TcpClient.Write(buffer)) {
    return false;
  }

  memset(buffer, 0, sizeof(buffer));
  // 接收服务端的回应报文.
  if (!TcpClient.Read(buffer)) {
    return false;
  }
  printf("接收: %s\n", buffer);

  // 解析服务端返回的 XML.
  int iretcode = -1;
  GetXMLBuffer(buffer, "retcode", &iretcode);
  if (iretcode != 0) {
    printf("登录失败.\n");
    return false;
  }

  printf("登录成功.\n");

  return true;
}

bool srv002() {
  char buffer[1024];

  SPRINTF(buffer, sizeof(buffer),
          "<srvcode>2</srvcode><cardid>62620000000001</cardid>");
  printf("发送: %s\n", buffer);
  // 向服务端发送请求报文.
  if (!TcpClient.Write(buffer)) {
    return false;
  }

  memset(buffer, 0, sizeof(buffer));
  // 接收服务端的回应报文.
  if (!TcpClient.Read(buffer)) {
    return false;
  }
  printf("接收: %s\n", buffer);

  // 解析服务端返回的 XML.
  int iretcode = -1;
  GetXMLBuffer(buffer, "retcode", &iretcode);
  if (iretcode != 0) {
    printf("查询余额失败.\n");
    return false;
  }

  double ye = 0;
  GetXMLBuffer(buffer, "ye", &ye);

  printf("查询余额成功 (%.2f).\n", ye);

  return true;
}
