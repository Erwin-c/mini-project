/*
 * demo20.cc, 此程序演示采用开发框架的 CTcpServer 类实现 Socket
 *            通讯多线程的服务端.
 *
 *  Author: Erwin
 */

#include "../../public/_public.h"

CLogFile logfile;      // 服务程序的运行日志.
CTcpServer TcpServer;  // 创建服务端对象.

pthread_spinlock_t vthidlock;  // 用于锁定 vthid 的自旋锁.
vector<pthread_t> vthid;       // 存放全部线程 id 的容器.

// 进程的退出函数.
void EXIT(int sig);

// 线程主函数.
void *thmain(void *arg);

// 线程清理函数.
void thcleanup(void *arg);

int main(int argc, char *argv[]) {
  if (argc != 3) {
    printf(
        "Using: ./demo20 port logfile\nExample: ./demo20 5005 "
        "~/Coding/mini-project/tmp/demo20.log\n\n");
    return -1;
  }

  // 关闭全部的信号和输入输出.
  // 设置信号, 在 shell 状态下可用 "kill + 进程号" 正常终止些进程.
  // 但请不要用 "kill -9 + 进程号" 强行终止.
  CloseIOAndSignal();
  signal(SIGINT, EXIT);
  signal(SIGTERM, EXIT);

  if (!logfile.Open(argv[2], "a+")) {
    printf("logfile.Open(%s) failed.\n", argv[2]);
    return -1;
  }

  // 服务端初始化.
  if (!TcpServer.InitServer(atoi(argv[1]))) {
    logfile.Write("TcpServer.InitServer(%s) failed.\n", argv[1]);
    return -1;
  }

  pthread_spin_init(&vthidlock, 0);

  while (true) {
    // 等待客户端的连接请求.
    if (!TcpServer.Accept()) {
      logfile.Write("TcpServer.Accept() failed.\n");
      EXIT(-1);
    }

    logfile.Write("Client (%s) has been connected.\n", TcpServer.GetIP());

    // 创建一个新的线程, 让它与客户端通讯.
    pthread_t thid;
    if (pthread_create(&thid, NULL, thmain, (void *)(long)TcpServer.m_connfd) !=
        0) {
      logfile.Write("pthread_create() failed.\n");
      TcpServer.CloseListen();
      continue;
    }

    pthread_spin_lock(&vthidlock);

    // 把线程 id 放入容器.
    vthid.push_back(thid);

    pthread_spin_unlock(&vthidlock);
  }

  return 0;
}

void *thmain(void *arg) {
  // 把线程清理函数入栈 (关闭客户端的 Socket).
  pthread_cleanup_push(thcleanup, arg);

  int connfd = (int)(long)arg;  // 客户端的 Socket.

  // 线程取消方式为立即取消.
  pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

  // 把线程分离出去.
  pthread_detach(pthread_self());

  // 子线程与客户端进行通讯, 处理业务.
  int ibuflen;
  char buffer[102400];

  // 与客户端通讯, 接收客户端发过来的报文后, 回复 ok.
  while (1) {
    memset(buffer, 0, sizeof(buffer));
    if (!TcpRead(connfd, buffer, &ibuflen, 30)) {
      break;
    }

    // 接收客户端的请求报文.
    logfile.Write("Receive: %s\n", buffer);

    strcpy(buffer, "ok");
    if (!TcpWrite(connfd, buffer)) {
      break;
    }

    // 向客户端发送响应结果.
    logfile.Write("Send: %s\n", buffer);
  }

  // 关闭客户端的连接.
  close(connfd);

  // 把本线程 id 从存放线程 id 的容器中删除.
  pthread_spin_lock(&vthidlock);

  for (size_t ii = 0; ii < vthid.size(); ++ii) {
    if (pthread_equal(pthread_self(), vthid[ii])) {
      vthid.erase(vthid.begin() + ii);
      break;
    }
  }

  pthread_spin_unlock(&vthidlock);

  // 把线程清理函数出栈.
  pthread_cleanup_pop(1);
}

void EXIT(int sig) {
  // 以下代码是为了防止信号处理函数在执行的过程中被信号中断.
  signal(SIGINT, SIG_IGN);
  signal(SIGTERM, SIG_IGN);

  logfile.Write("Process exited, sig = %d.\n", sig);

  // 关闭监听的 Socket.
  TcpServer.CloseListen();

  // 取消全部的线程.
  for (size_t ii = 0; ii < vthid.size(); ++ii) {
    pthread_cancel(vthid[ii]);
  }

  // 让子线程有足够的时间退出.
  sleep(1);

  pthread_spin_destroy(&vthidlock);

  exit(0);
}

void thcleanup(void *arg) {
  // 关闭客户端的 Socket.
  close((int)(long)arg);

  logfile.Write("Thread %lu exited.\n", pthread_self());
}
