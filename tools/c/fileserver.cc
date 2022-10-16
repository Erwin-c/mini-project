/*
 * fileserver.cc, 文件传输的服务端.
 *
 *  Author: Erwin
 */

#include "_public.h"

CLogFile logfile;      // 服务程序的运行日志.
CTcpServer TcpServer;  // 创建服务端对象.

char strrecvbuffer[1024];  // 发送报文的 buffer.
char strsendbuffer[1024];  // 接收报文的 buffer.

void FathEXIT(int sig);  // 父进程退出函数.
void ChldEXIT(int sig);  // 子进程退出函数.

// 处理业务的主函数.
bool _main(const char *strrecvbuffer, char *strsendbuffer);

// 心跳.
bool srv000(const char *strrecvbuffer, char *strsendbuffer);

// 登录业务处理函数.
bool srv001(const char *strrecvbuffer, char *strsendbuffer);

int main(int argc, char *argv[]) {
  if (argc != 4) {
    printf("Using: ./fileserver port logfile\n");
    printf(
        "Example: ./fileserver 5005 "
        "~/Coding/mini-project/log/idc/fileserver.log 35\n");
    printf(
        "~/Coding/mini-project/tools/bin/procctl 10 "
        "~/Coding/mini-project/tools/bin/fileserver 5005 "
        "~/Coding/mini-project/log/idc/fileserver.log\n\n\n");
    return -1;
  }

  // 关闭全部的信号和输入输出.
  // 设置信号, 在 shell 状态下可用 "kill + 进程号" 正常终止进程.
  // 但请不要用 "kill -9 + 进程号" 强行终止.
  CloseIOAndSignal();
  signal(SIGINT, FathEXIT);
  signal(SIGTERM, FathEXIT);

  if (!logfile.Open(argv[2], "a+")) {
    printf("logfile.Open(%s) failed.\n", argv[2]);
    return -1;
  }

  // 服务端初始化.
  if (!TcpServer.InitServer(atoi(argv[1]))) {
    logfile.Write("TcpServer.InitServer(%s) failed.\n", argv[1]);
    return -1;
  }

  while (true) {
    // 等待客户端的连接请求.
    if (!TcpServer.Accept()) {
      logfile.Write("TcpServer.Accept() failed.\n");
      FathEXIT(-1);
    }

    logfile.Write("Client (%s) has been connected.\n", TcpServer.GetIP());

    /*
    // 父进程继续回到 Accept().
    if (fork() > 0) {
      TcpServer.CloseClient();
      continue;
    }

    // 子进程重新设置退出信号.
    signal(SIGINT, ChldEXIT);
    signal(SIGTERM, ChldEXIT);

    TcpServer.CloseListen();
    */

    // 子进程与客户端进行通讯, 处理业务.

    // 与客户端通讯，接收客户端发过来的报文后，回复ok。
    while (1) {
      memset(strrecvbuffer, 0, sizeof(strrecvbuffer));
      memset(strsendbuffer, 0, sizeof(strsendbuffer));

      // 接收客户端的请求报文.
      if (!TcpServer.Read(strrecvbuffer, atoi(argv[3]))) {
        break;
      }
      logfile.Write("Receive: %s\n", strrecvbuffer);

      // 处理业务的主函数。
      if (!_main(strrecvbuffer, strsendbuffer)) {
        break;
      }

      // 向客户端发送响应结果.
      if (!TcpServer.Write(strsendbuffer)) {
        break;
      }
      logfile.Write("Send: %s\n", strsendbuffer);
    }

    ChldEXIT(0);
  }
}

void FathEXIT(int sig) {
  // 以下代码是为了防止信号处理函数在执行的过程中被信号中断.
  signal(SIGINT, SIG_IGN);
  signal(SIGTERM, SIG_IGN);

  logfile.Write("Father process exited, sig = %d.\n", sig);

  TcpServer.CloseListen();  // 关闭监听的 Socket.

  kill(0, 15);  // 通知全部的子进程退出.

  exit(0);
}

void ChldEXIT(int sig) {
  // 以下代码是为了防止信号处理函数在执行的过程中被信号中断.
  signal(SIGINT, SIG_IGN);
  signal(SIGTERM, SIG_IGN);

  logfile.Write("Child process exited, sig = %d.\n\n", sig);

  TcpServer.CloseClient();  // 关闭客户端的 Socket.

  exit(0);
}

bool _main(const char *strrecvbuffer, char *strsendbuffer) {
  // 解析 strrecvbuffer, 获取服务代码 (业务代码).
  int isrvcode = -1;
  GetXMLBuffer(strrecvbuffer, "srvcode", &isrvcode);

  // 处理每种业务.
  switch (isrvcode) {
    case 0:  // 心跳.
      srv000(strrecvbuffer, strsendbuffer);
      break;
    case 1:  // 登录.
      srv001(strrecvbuffer, strsendbuffer);
      break;
    default:
      logfile.Write("Service code is illegal: %s\n", strrecvbuffer);
      return false;
  }

  return true;
}

bool srv000(const char *strrecvbuffer, char *strsendbuffer) {
  strcpy(strsendbuffer, "<retcode>0</retcode><message>Succeeded.</message>");

  return true;
}

bool srv001(const char *strrecvbuffer, char *strsendbuffer) {
  // <srvcode>1</srvcode><tel>1392220000</tel><password>123456</password>

  // 解析 strrecvbuffer, 获取业务参数.
  char tel[21], password[31];
  GetXMLBuffer(strrecvbuffer, "tel", tel, 20);
  GetXMLBuffer(strrecvbuffer, "password", password, 30);

  // 处理业务.
  // 把处理结果生成 strsendbuffer.
  if (strcmp(tel, "1392220000") == 0 && strcmp(password, "123456") == 0) {
    strcpy(strsendbuffer, "<retcode>0</retcode><message>Succeeded.</message>");
  } else {
    strcpy(strsendbuffer, "<retcode>-1</retcode><message>Falied.</message>");
  }

  return true;
}
