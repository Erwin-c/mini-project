/*
 * tcpputfiles.cc, 采用 TCP 协议, 实现文件上传的客户端.
 *
 *  Author: Erwin
 */

#include "_public.h"

// 程序运行的参数结构体.
struct st_arg {
  int clienttype;  // 客户端类型, 1-上传文件; 2-下载文件.
  char ip[31];     // 服务端的 IP 地址.
  int port;        // 服务端的端口.
  int ptype;  // 文件上传成功后本地文件的处理方式: 1-删除文件; 2-移动到备份目录.
  char clientpath[301];  // 本地文件存放的根目录.
  char clientpathbak[301];  // 文件成功上传后, 本地文件备份的根目录,
                            // 当 ptype == 2 时有效.
  bool andchild;  // 是否上传 clientpath 目录下各级子目录的文件,
                  // true-是; false-否.
  char matchname[301];  // 待上传文件名的匹配规则, 如 "*.TXT, *.XML".
  char srvpath[301];    // 服务端文件存放的根目录.
  int timetvl;          // 扫描本地目录文件的时间间隔, 单位: 秒.
  int timeout;          // 进程心跳的超时时间.
  char pname[51];       // 进程名, 建议用 "tcpputfiles_后缀" 的方式.
};

CTcpClient TcpClient;

bool srv000();  // 心跳.
bool srv001();  // 登录业务.

int main(int argc, char *argv[]) {
  if (argc != 3) {
    printf(
        "Using:. /tcpputfiles ip port\nExample: ./tcpputfiles 127.0.0.1 "
        "5005\n\n");
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

  sleep(3);

  for (int ii = 3; ii < 5; ++ii) {
    if (!srv000()) {
      break;
    }

    sleep(ii);
  }

  return 0;
}

bool srv000() {
  char buffer[1024];

  SPRINTF(buffer, sizeof(buffer), "<srvcode>0</srvcode>");
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

  return true;
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
