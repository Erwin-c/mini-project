/*
 * demo11.cc, 此程序用于演示网银 APP 软件的客户端.
 *
 *  Author: Erwin
 */

#include "../../_public.h"

CTcpClient TcpClient;

bool srv001();  // 登录业务.
bool srv002();  // 我的账户 (查询余额).

int main(int argc, char *argv[]) {
  if (argc != 3) {
    printf("Using:. /demo11 ip port\nExample: ./demo11 127.0.0.1 5005\n\n");
    return -1;
  }

  // 向服务端发起连接请求.
  if (!TcpClient.ConnectToServer(argv[1], atoi(argv[2]))) {
    printf("TcpClient.ConnectToServer(%s, %s) failed.\n", argv[1], argv[2]);
    return -1;
  }

  // 登录业务.
  if (!srv001()) {
    printf("srv001() failed.\n");
    return -1;
  }

  // 我的账户 (查询余额).
  if (!srv002()) {
    printf("srv002() failed.\n");
    return -1;
  }

  return 0;
}

bool srv001() {
  char buffer[1024];

  SPRINTF(
      buffer, sizeof(buffer),
      "<srvcode>1</srvcode><tel>1392220000</tel><password>123456</password>");
  printf("Send: %s\n", buffer);
  // 向服务端发送请求报文.
  if (!TcpClient.Write(buffer)) {
    return false;
  }

  memset(buffer, 0, sizeof(buffer));
  // 接收服务端的回应报文.
  if (!TcpClient.Read(buffer)) {
    return false;
  }
  printf("Receive: %s\n", buffer);

  // 解析服务端返回的 XML.
  int iretcode = -1;
  GetXMLBuffer(buffer, "retcode", &iretcode);
  if (iretcode != 0) {
    printf("Login failed.\n");
    return false;
  }

  printf("Login succeeded.\n");

  return true;
}

bool srv002() {
  char buffer[1024];

  SPRINTF(buffer, sizeof(buffer),
          "<srvcode>2</srvcode><cardid>62620000000001</cardid>");
  printf("Send: %s\n", buffer);
  // 向服务端发送请求报文.
  if (!TcpClient.Write(buffer)) {
    return false;
  }

  memset(buffer, 0, sizeof(buffer));
  // 接收服务端的回应报文.
  if (!TcpClient.Read(buffer)) {
    return false;
  }
  printf("Receive: %s\n", buffer);

  // 解析服务端返回的 XML.
  int iretcode = -1;
  GetXMLBuffer(buffer, "retcode", &iretcode);
  if (iretcode != 0) {
    printf("To query balance failed.\n");
    return false;
  }

  double ye = 0;
  GetXMLBuffer(buffer, "ye", &ye);

  printf("To query balance succeeded(%.2f).\n", ye);

  return true;
}
