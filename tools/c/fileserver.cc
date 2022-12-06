/*
 * fileserver.cc, 文件传输的服务端.
 *
 *  Author: Erwin
 */

#include "_public.h"

// 程序运行的参数结构体.
struct st_arg {
  int clienttype;  // 客户端类型, 1-上传文件; 2-下载文件.
  char ip[31];     // 服务端的 IP 地址.
  int port;        // 服务端的端口.
  int ptype;  // 文件成功传输后的处理方式: 1-删除文件; 2-移动到备份目录.
  char clientpath[301];  // 客户端文件存放的根目录.
  bool andchild;  // 是否传输各级子目录的文件, true-是; false-否.
  char matchname[301];  // 待传输文件名的匹配规则, 如 "*.TXT, *.XML".
  char srvpath[301];    // 服务端文件存放的根目录.
  char srvpathbak[301];  // 服务端文件存放的根目录.
  int timetvl;           // 扫描目录文件的时间间隔, 单位: 秒.
  int timeout;           // 进程心跳的超时时间.
  char pname[51];        // 进程名, 建议用 "tcpgetfiles_后缀" 的方式.
} starg;

// 如果调用 _tcpputfiles() 发送了文件, bcontinue 为 true, 初始化为 true.
bool bcontinue = true;

char strrecvbuffer[1024];  // 发送报文的 buffer.

char strsendbuffer[1024];  // 接收报文的 buffer.

CLogFile logfile;  // 服务程序的运行日志.

CTcpServer TcpServer;  // Socket 通讯的服务端类.

CPActive PActive;  // 进程心跳。

// 登录业务处理函数.
bool ClientLogin();

// 心跳.
bool ActiveTest();

// 接收文件的内容.
bool RecvFile(const int sockfd, const char *filename, const char *mtime,
              int filesize);

// 上传文件的主函数.
void RecvFilesMain();

// 把文件的内容发送给对端.
bool SendFile(const int sockfd, const char *filename, const int filesize);

// 删除或者转存本地的文件.
bool AckMessage(const char *strrecvbuffer);

// 下载文件的主函数.
void SendFilesMain();

// 文件下载的主函数, 执行一次文件下载的任务.
bool _tcpputfiles();

// 把 XML 解析到参数 starg 结构中.
bool _xmltoarg(char *strxmlbuffer);

// 程序帮助文档.
void _help();

// 父进程退出函数.
void FathEXIT(int sig);

// 子进程退出函数.
void ChldEXIT(int sig);

int main(int argc, char *argv[]) {
  if (argc != 3) {
    _help();
    return -1;
  }

  // 关闭全部的信号和输入输出.
  // 设置信号, 在 shell 状态下可用 "kill + 进程号" 正常终止些进程.
  // 但请不要用 "kill -9 +进程号" 强行终止.
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

    // 处理登录客户端的登录报文.
    if (!ClientLogin()) {
      ChldEXIT(-1);
    }

    // 如果 clienttype == 1, 调用上传文件的主函数.
    if (starg.clienttype == 1) {
      RecvFilesMain();
    }

    // 如果clienttype == 2, 调用下载文件的主函数.
    if (starg.clienttype == 2) {
      SendFilesMain();
    }

    ChldEXIT(0);
  }

  return 0;
}

bool ClientLogin() {
  memset(strrecvbuffer, 0, sizeof(strrecvbuffer));
  memset(strsendbuffer, 0, sizeof(strsendbuffer));

  if (!TcpServer.Read(strrecvbuffer, 20)) {
    logfile.Write("TcpServer.Read() 失败.\n");
    return false;
  }
  logfile.Write("strrecvbuffer = %s\n", strrecvbuffer);

  // 解析客户端登录报文.
  _xmltoarg(strrecvbuffer);

  if (starg.clienttype != 1 && starg.clienttype != 2) {
    strcpy(strsendbuffer, "失败.");
  } else {
    strcpy(strsendbuffer, "成功.");
  }

  if (!TcpServer.Write(strsendbuffer)) {
    logfile.Write("TcpServer.Write() 失败.\n");
    return false;
  }

  logfile.Write("%s 登录 %s.\n", TcpServer.GetIP(), strsendbuffer);

  return true;
}

bool ActiveTest() {
  memset(strsendbuffer, 0, sizeof(strsendbuffer));
  memset(strrecvbuffer, 0, sizeof(strrecvbuffer));

  SPRINTF(strsendbuffer, sizeof(strsendbuffer), "<activetest>ok</activetest>");
  logfile.Write("发送: %s\n", strsendbuffer);

  // 向客户端发送请求报文.
  if (!TcpServer.Write(strsendbuffer)) {
    return false;
  }

  // 接收客户端的回应报文.
  if (!TcpServer.Read(strrecvbuffer, 20)) {
    return false;
  }

  logfile.Write("接收: %s\n", strrecvbuffer);

  return true;
}

bool RecvFile(const int sockfd, const char *filename, const char *mtime,
              int filesize) {
  // 生成临时文件名.
  char strfilenametmp[301];
  SNPRINTF(strfilenametmp, sizeof(strfilenametmp), 300, "%s.tmp", filename);

  int totalbytes = 0;  // 已接收文件的总字节数.
  int onread = 0;      // 本次打算接收的字节数.
  char buffer[1000];   // 接收文件内容的缓冲区.
  FILE *fp = NULL;

  // 创建临时文件。
  if ((fp = FOPEN(strfilenametmp, "wb")) == NULL) {
    return false;
  }

  while (true) {
    memset(buffer, 0, sizeof(buffer));

    // 计算本次应该接收的字节数.
    if (filesize - totalbytes > 1000) {
      onread = 1000;
    } else {
      onread = filesize - totalbytes;
    }

    // 接收文件内容.
    if (!Readn(sockfd, buffer, onread)) {
      fclose(fp);
      return false;
    }

    // 把接收到的内容写入文件.
    fwrite(buffer, 1, onread, fp);

    // 计算已接收文件的总字节数, 如果文件接收完, 跳出循环.
    totalbytes = totalbytes + onread;

    if (totalbytes == filesize) {
      break;
    }
  }

  // 关闭临时文件.
  fclose(fp);

  // 重置文件的时间.
  UTime(strfilenametmp, mtime);

  // 把临时文件RENAME为正式的文件.
  if (!RENAME(strfilenametmp, filename)) {
    return false;
  }

  return true;
}

void RecvFilesMain() {
  PActive.AddPInfo(starg.timeout, starg.pname);

  while (true) {
    memset(strsendbuffer, 0, sizeof(strsendbuffer));
    memset(strrecvbuffer, 0, sizeof(strrecvbuffer));

    PActive.UptATime();

    // 接收客户端的报文.
    // 第二个参数的取值必须大于 starg.timetvl, 小于starg.timeout.
    if (!TcpServer.Read(strrecvbuffer, starg.timetvl + 10)) {
      logfile.Write("TcpServer.Read() 失败.\n");
      return;
    }
    logfile.Write("strrecvbuffer = %s\n", strrecvbuffer);

    // 处理心跳报文.
    if (strcmp(strrecvbuffer, "<activetest>ok</activetest>") == 0) {
      strcpy(strsendbuffer, "ok");
      logfile.Write("strsendbuffer = %s\n", strsendbuffer);

      if (!TcpServer.Write(strsendbuffer)) {
        logfile.Write("TcpServer.Write() 失败.\n");
        return;
      }
    }

    // 处理上传文件的请求报文.
    if (strncmp(strrecvbuffer, "<filename>", 10) == 0) {
      // 解析上传文件请求报文的 XML.
      char clientfilename[301];
      memset(clientfilename, 0, sizeof(clientfilename));
      char mtime[21];
      memset(mtime, 0, sizeof(mtime));
      int filesize = 0;
      GetXMLBuffer(strrecvbuffer, "filename", clientfilename, 300);
      GetXMLBuffer(strrecvbuffer, "mtime", mtime, 19);
      GetXMLBuffer(strrecvbuffer, "size", &filesize);

      // 客户端和服务端文件的目录是不一样的, 以下代码生成服务端的文件名.
      // 把文件名中的 clientpath 替换成 srvpath, 要小心第三个参数.
      char serverfilename[301];
      memset(serverfilename, 0, sizeof(serverfilename));
      strcpy(serverfilename, clientfilename);
      UpdateStr(serverfilename, starg.clientpath, starg.srvpath, false);

      // 接收文件的内容.
      logfile.Write("接收 %s (%d) ...", serverfilename, filesize);
      if (RecvFile(TcpServer.m_connfd, serverfilename, mtime, filesize)) {
        logfile.WriteEx("成功.\n");
        SNPRINTF(strsendbuffer, sizeof(strsendbuffer), 1000,
                 "<filename>%s</filename><result>ok</result>", clientfilename);
      } else {
        logfile.WriteEx("失败.\n");
        SNPRINTF(strsendbuffer, sizeof(strsendbuffer), 1000,
                 "<filename>%s</filename><result>failed</result>",
                 clientfilename);
      }

      // 把接收结果返回给对端.
      logfile.Write("strsendbuffer = %s\n", strsendbuffer);
      if (!TcpServer.Write(strsendbuffer)) {
        logfile.Write("TcpServer.Write() 失败.\n");
        return;
      }
    }
  }

  return;
}

bool SendFile(const int sockfd, const char *filename, const int filesize) {
  int onread = 0;      // 每次调用 fread() 时打算读取的字节数.
  int bytes = 0;       // 调用一次 fread() 从文件中读取的字节数.
  char buffer[1000];   // 存放读取数据的 buffer.
  int totalbytes = 0;  // 从文件中已读取的字节总数.
  FILE *fp = NULL;

  // 以 "rb" 的模式打开文件.
  if ((fp = fopen(filename, "rb")) == NULL) {
    return false;
  }

  while (true) {
    memset(buffer, 0, sizeof(buffer));

    // 计算本次应该读取的字节数, 如果剩余的数据超过 1000 字节,
    // 就打算读 1000 字节.
    if (filesize - totalbytes > 1000) {
      onread = 1000;
    } else {
      onread = filesize - totalbytes;
    }

    // 从文件中读取数据.
    bytes = fread(buffer, 1, onread, fp);

    // 把读取到的数据发送给对端.
    if (bytes > 0) {
      if (!Writen(sockfd, buffer, bytes)) {
        fclose(fp);
        return false;
      }
    }

    // 计算文件已读取的字节总数, 如果文件已读完, 跳出循环.
    totalbytes = totalbytes + bytes;

    if (totalbytes == filesize) {
      break;
    }
  }

  fclose(fp);

  return true;
}

bool AckMessage(const char *strrecvbuffer) {
  char filename[301];
  char result[11];

  memset(filename, 0, sizeof(filename));
  memset(result, 0, sizeof(result));

  GetXMLBuffer(strrecvbuffer, "filename", filename, 300);
  GetXMLBuffer(strrecvbuffer, "result", result, 10);

  // 如果客户端接收文件不成功, 直接返回.
  if (strcmp(result, "ok") != 0) {
    return true;
  }

  // ptype == 1, 删除文件.
  if (starg.ptype == 1) {
    if (!REMOVE(filename)) {
      logfile.Write("REMOVE(%s) 失败.\n", filename);
      return false;
    }
  }

  // ptype == 2, 移动到备份目录.
  if (starg.ptype == 2) {
    // 生成转存后的备份目录文件名.
    char bakfilename[301];
    STRCPY(bakfilename, sizeof(bakfilename), filename);
    UpdateStr(bakfilename, starg.srvpath, starg.srvpathbak, false);
    if (!RENAME(filename, bakfilename)) {
      logfile.Write("RENAME(%s, %s) 失败.\n", filename, bakfilename);
      return false;
    }
  }

  return true;
}

void SendFilesMain() {
  PActive.AddPInfo(starg.timeout, starg.pname);

  while (true) {
    // 调用文件下载的主函数, 执行一次文件下载的任务.
    if (!_tcpputfiles()) {
      logfile.Write("_tcpputfiles() 失败.\n");
      return;
    }

    if (!bcontinue) {
      sleep(starg.timetvl);

      if (!ActiveTest()) {
        break;
      }
    }

    PActive.UptATime();
  }

  return;
}

bool _tcpputfiles() {
  CDir Dir;

  // 调用 OpenDir() 打开 starg.srvpath 目录.
  if (!Dir.OpenDir(starg.srvpath, starg.matchname, 10000, starg.andchild)) {
    logfile.Write("Dir.OpenDir(%s) 失败.\n", starg.srvpath);
    return false;
  }

  int delayed = 0;  // 未收到对端确认报文的文件数量.
  int buflen = 0;   // 用于存放 strrecvbuffer 的长度.

  bcontinue = false;

  while (true) {
    memset(strsendbuffer, 0, sizeof(strsendbuffer));
    memset(strrecvbuffer, 0, sizeof(strrecvbuffer));

    // 遍历目录中的每个文件, 调用 ReadDir() 获取一个文件名.
    if (!Dir.ReadDir()) {
      break;
    }

    bcontinue = true;

    // 把文件名, 修改时间, 文件大小组成报文, 发送给对端.
    SNPRINTF(strsendbuffer, sizeof(strsendbuffer), 1000,
             "<filename>%s</filename><mtime>%s</mtime><size>%d</size>",
             Dir.m_FullFileName, Dir.m_ModifyTime, Dir.m_FileSize);

    logfile.Write("strsendbuffer = %s\n", strsendbuffer);
    if (!TcpServer.Write(strsendbuffer)) {
      logfile.Write("TcpServer.Write() 失败.\n");
      return false;
    }

    // 把文件的内容发送给对端.
    logfile.Write("发送 %s(%d) ...", Dir.m_FullFileName, Dir.m_FileSize);
    if (SendFile(TcpServer.m_connfd, Dir.m_FullFileName, Dir.m_FileSize)) {
      logfile.WriteEx("成功.\n");
      ++delayed;
    } else {
      logfile.WriteEx("失败.\n");
      TcpServer.CloseClient();
      return false;
    }

    PActive.UptATime();

    // 接收对端的确认报文.
    while (delayed > 0) {
      memset(strrecvbuffer, 0, sizeof(strrecvbuffer));
      if (!TcpRead(TcpServer.m_connfd, strrecvbuffer, &buflen, -1)) {
        break;
      }
      logfile.Write("strrecvbuffer = %s\n", strrecvbuffer);

      // 删除或者转存本地的文件.
      --delayed;
      AckMessage(strrecvbuffer);
    }
  }

  // 继续接收对端的确认报文.
  while (delayed > 0) {
    memset(strrecvbuffer, 0, sizeof(strrecvbuffer));
    if (!TcpRead(TcpServer.m_connfd, strrecvbuffer, &buflen, 10)) {
      break;
    }
    logfile.Write("strrecvbuffer = s\n", strrecvbuffer);

    // 删除或者转存本地的文件.
    --delayed;
    AckMessage(strrecvbuffer);
  }

  return true;
}

bool _xmltoarg(char *strxmlbuffer) {
  memset(&starg, 0, sizeof(st_arg));

  // 不需要对参数做合法性判断, 客户端已经判断过了.
  GetXMLBuffer(strxmlbuffer, "clienttype", &starg.clienttype);
  GetXMLBuffer(strxmlbuffer, "ptype", &starg.ptype);
  GetXMLBuffer(strxmlbuffer, "clientpath", starg.clientpath);
  GetXMLBuffer(strxmlbuffer, "andchild", &starg.andchild);
  GetXMLBuffer(strxmlbuffer, "matchname", starg.matchname);
  GetXMLBuffer(strxmlbuffer, "srvpath", starg.srvpath);
  GetXMLBuffer(strxmlbuffer, "srvpathbak", starg.srvpathbak);

  GetXMLBuffer(strxmlbuffer, "timetvl", &starg.timetvl);
  if (starg.timetvl > 30) {
    starg.timetvl = 30;
  }

  GetXMLBuffer(strxmlbuffer, "timeout", &starg.timeout);
  if (starg.timeout < 50) {
    starg.timeout = 50;
  }

  GetXMLBuffer(strxmlbuffer, "pname", starg.pname, 50);
  strcat(starg.pname, "_srv");

  return true;
}

void _help() {
  printf("\n");

  printf(
      "Using: /home/erwin/Coding/mini-project/tools/bin/fileserver port "
      "logfile\n");
  printf(
      "Example: /home/erwin/Coding/mini-project/tools/bin/procctl 10 "
      "/home/erwin/Coding/mini-project/tools/bin/fileserver 5005 "
      "/home/erwin/Coding/mini-project/log/idc/fileserver.log\n");
  printf(
      "         /home/erwin/Coding/mini-project/tools/bin/fileserver 5005 "
      "/home/erwin/Coding/mini-project/log/idc/fileserver.log\n\n\n");

  return;
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
