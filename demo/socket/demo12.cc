/*
 * demo12.cc, 此程序用于演示网银 APP 软件的服务端.
 *
 *  Author: Erwin
 */

#include "_public.h"

// 客户端是否已登录: true-已登录; false-未登录或登录失败.
bool bsession = false;

CLogFile logfile;  // 服务程序的运行日志.

CTcpServer TcpServer;  // 创建服务端对象.

// 登录业务处理函数.
bool srv001(const char *strrecvbuffer, char *strsendbuffer);

// 查询余额业务处理函数.
bool srv002(const char *strrecvbuffer, char *strsendbuffer);

// 处理业务的主函数.
bool _main(const char *strrecvbuffer, char *strsendbuffer);

// 父进程退出函数.
void FathEXIT(int sig);

// 子进程退出函数.
void ChldEXIT(int sig);

int main(int argc, char *argv[]) {
  if (argc != 3) {
    printf(
        "Using: ./demo12 port logfile\nExample: ./demo12 5005 "
        "/home/erwin/Coding/mini-project/log/demo/socket/demo12.log\n\n");
    return -1;
  }

  // 关闭全部的信号和输入输出.
  // 设置信号, 在 shell 状态下可用 "kill + 进程号" 正常终止进程.
  // 但请不要用 "kill -9 + 进程号" 强行终止.
  CloseIOAndSignal();

  signal(SIGINT, FathEXIT);
  signal(SIGTERM, FathEXIT);

  if (!logfile.Open(argv[2], "a+")) {
    printf("logfile.Open(%s) 失败.\n", argv[2]);
    return -1;
  }

  // 服务端初始化.
  if (!TcpServer.InitServer(atoi(argv[1]))) {
    logfile.Write("TcpServer.InitServer(%s) 失败.\n", argv[1]);
    return -1;
  }

  while (true) {
    // 等待客户端的连接请求.
    if (!TcpServer.Accept()) {
      logfile.Write("TcpServer.Accept() 失败.\n");
      FathEXIT(-1);
    }

    logfile.Write("客户端 (%s) 已连接.\n", TcpServer.GetIP());

    // 父进程继续回到 Accept().
    if (fork() > 0) {
      TcpServer.CloseClient();
      continue;
    }

    // 子进程重新设置退出信号.
    signal(SIGINT, ChldEXIT);
    signal(SIGTERM, ChldEXIT);

    TcpServer.CloseListen();

    // 子进程与客户端进行通讯, 处理业务.
    char strrecvbuffer[1024], strsendbuffer[1024];

    // 与客户端通讯, 接收客户端发过来的报文后, 回复 ok.
    while (1) {
      memset(strrecvbuffer, 0, sizeof(strrecvbuffer));
      memset(strsendbuffer, 0, sizeof(strsendbuffer));

      // 接收客户端的请求报文.
      if (!TcpServer.Read(strrecvbuffer)) {
        break;
      }
      logfile.Write("接收: %s\n", strrecvbuffer);

      // 处理业务的主函数。
      if (!_main(strrecvbuffer, strsendbuffer)) {
        break;
      }

      // 向客户端发送响应结果.
      if (!TcpServer.Write(strsendbuffer)) {
        break;
      }
      logfile.Write("发送: %s\n", strsendbuffer);
    }

    ChldEXIT(0);
  }

  return 0;
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
    strcpy(strsendbuffer, "<retcode>0</retcode><message>成功.</message>");
    bsession = true;
  } else {
    strcpy(strsendbuffer, "<retcode>-1</retcode><message>失败.</message>");
  }

  return true;
}

bool srv002(const char *strrecvbuffer, char *strsendbuffer) {
  // <srvcode>2</srvcode><cardid>62620000000001</cardid>

  // 解析 strrecvbuffer, 获取业务参数.
  char cardid[31];
  GetXMLBuffer(strrecvbuffer, "cardid", cardid, 30);

  // 处理业务.
  // 把处理结果生成 strsendbuffer.
  if (strcmp(cardid, "62620000000001") == 0) {
    strcpy(strsendbuffer,
           "<retcode>0</retcode><message>成功.</message><ye>100.58</ye>");
  } else {
    strcpy(strsendbuffer, "<retcode>-1</retcode><message>成功.</message>");
  }

  return true;
}

bool _main(const char *strrecvbuffer, char *strsendbuffer) {
  // 解析 strrecvbuffer, 获取服务代码 (业务代码).
  int isrvcode = -1;
  GetXMLBuffer(strrecvbuffer, "srvcode", &isrvcode);

  if (isrvcode != 1 && !bsession) {
    strcpy(strsendbuffer,
           "<retcode>-1</retcode><message>用户未登录.</message>");
    return true;
  }

  // 处理每种业务.
  switch (isrvcode) {
    case 1:  // 登录.
      srv001(strrecvbuffer, strsendbuffer);
      break;
    case 2:  // 查询余额.
      srv002(strrecvbuffer, strsendbuffer);
      break;
    default:
      logfile.Write("业务代码不合法: %s\n", strrecvbuffer);
      return false;
  }

  return true;
}

void FathEXIT(int sig) {
  // 以下代码是为了防止信号处理函数在执行的过程中被信号中断.
  signal(SIGINT, SIG_IGN);
  signal(SIGTERM, SIG_IGN);

  logfile.Write("父进程退出, sig = %d.\n", sig);

  // 关闭监听的 Socket.
  TcpServer.CloseListen();

  // 通知全部的子进程退出.
  kill(0, 15);

  exit(0);
}

void ChldEXIT(int sig) {
  // 以下代码是为了防止信号处理函数在执行的过程中被信号中断.
  signal(SIGINT, SIG_IGN);
  signal(SIGTERM, SIG_IGN);

  logfile.Write("子进程退出, sig = %d.\n", sig);

  // 关闭客户端的 Socket.
  TcpServer.CloseClient();

  exit(0);
}
