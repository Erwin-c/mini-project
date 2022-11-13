/*
 * demo10.cc, 此程序演示采用开发框架的 CTcpServer 类实现 Socket
 *            通讯多进程的服务端.
 * 1) 在多进程的服务程序中, 如果杀掉一个子进程, 和这个子进程通讯的客户端会断开,
 *    但是, 不会影响其它的子进程和客户端, 也不会影响父进程.
 * 2) 如果杀掉父进程, 不会影响正在通讯中的子进程, 但是, 新的客户端无法建立连接.
 * 3) 如果用 killall + 程序名, 可以杀掉父进程和全部的子进程.
 *
 * 多进程网络服务端程序退出的三种情况:
 * 1) 如果是子进程收到退出信号, 该子进程断开与客户端连接的 Socket, 然后退出.
 * 2）如果是父进程收到退出信号, 父进程先关闭监听的 Socket,
 *    然后向全部的子进程发出退出信号.
 * 3) 如果父子进程都收到退出信号, 本质上与第 2 种情况相同.
 *
 *  Author: Erwin
 */

#include "../../public/_public.h"

CLogFile logfile;      // 服务程序的运行日志.
CTcpServer TcpServer;  // 创建服务端对象.

void FathEXIT(int sig);  // 父进程退出函数.
void ChldEXIT(int sig);  // 子进程退出函数.

int main(int argc, char *argv[]) {
  if (argc != 3) {
    printf(
        "Using: ./demo10 port logfile\nExample: ./demo10 5005 "
        "~/Coding/mini-project/tmp/demo10.log\n\n");
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
    char buffer[102400];

    // 与客户端通讯, 接收客户端发过来的报文后, 回复 Ok.
    while (1) {
      memset(buffer, 0, sizeof(buffer));
      // 接收客户端的请求报文.
      if (!TcpServer.Read(buffer)) {
        break;
      }
      logfile.Write("Receive: %s\n", buffer);

      strcpy(buffer, "Ok");
      // 向客户端发送响应结果.
      if (!TcpServer.Write(buffer)) {
        break;
      }
      logfile.Write("Send: %s\n", buffer);
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

  logfile.Write("Child process exited, sig = %d.\n", sig);

  TcpServer.CloseClient();  // 关闭客户端的 Socket.

  exit(0);
}
