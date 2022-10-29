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
} starg;

CLogFile logfile;

CTcpClient TcpClient;

CPActive PActive;  // 进程心跳.

char strrecvbuffer[1024];  // 发送报文的 buffer.
char strsendbuffer[1024];  // 接收报文的 buffer.

bool bcontinue = true;  // 如果调用 _tcpputfiles() 发送了文件,
                        // bcontinue 为 true, 初始化为 true.

// 程序退出和信号 2, 15 的处理函数.
void EXIT(int sig);

void _help();

// 把 XML 解析到参数 starg 结构中.
bool _xmltoarg(char *strxmlbuffer);

// 登录业务.
bool Login(const char *argv);

// 心跳.
bool ActiveTest();

// 文件上传的主函数, 执行一次文件上传的任务.
bool _tcpputfiles();

// 把文件的内容发送给对端.
bool SendFile(const int sockfd, const char *filename, const int filesize);

// 删除或者转存本地的文件.
bool AckMessage(const char *strrecvbuffer);

int main(int argc, char *argv[]) {
  if (argc != 3) {
    _help();
    return -1;
  }

  // 关闭全部的信号和输入输出.
  // 设置信号, 在 shell 状态下可用 "kill + 进程号" 正常终止些进程.
  // 但请不要用 "kill -9 + 进程号" 强行终止.
  CloseIOAndSignal();
  signal(SIGINT, EXIT);
  signal(SIGTERM, EXIT);

  // 打开日志文件.
  if (!logfile.Open(argv[1], "a+")) {
    printf("loggile.Open(%s) falied.\n", argv[1]);
    return -1;
  }

  // 解析 XML, 得到程序运行的参数.
  if (!_xmltoarg(argv[2])) {
    return -1;
  }

  // 把进程的心跳信息写入共享内存.
  PActive.AddPInfo(starg.timeout, starg.pname);

  // 向服务端发起连接请求.
  if (!TcpClient.ConnectToServer(starg.ip, starg.port)) {
    logfile.Write("TcpClient.ConnectToServer(%s, %d) failed.\n", starg.ip,
                  starg.port);
    EXIT(-1);
  }

  // 登录业务.
  if (!Login(argv[2])) {
    logfile.Write("Login() failed.\n");
    EXIT(-1);
  }

  while (true) {
    // 调用文件上传的主函数, 执行一次文件上传的任务.
    if (!_tcpputfiles()) {
      logfile.Write("_tcpputfiles() failed.\n");
      EXIT(-1);
    }

    if (!bcontinue) {
      sleep(starg.timetvl);

      if (!ActiveTest()) {
        break;
      }
    }

    PActive.UptATime();
  }

  EXIT(0);
}

void EXIT(int sig) {
  logfile.Write("Program exited, sig = %d\n\n", sig);

  exit(0);
}

void _help() {
  printf("\n");
  printf(
      "Using: ~/Coding/mini-project/tools/bin/tcpputfiles logfilename "
      "xmlbuffer\n\n");

  printf(
      "Sample: ~/Coding/mini-project/tools/bin/procctl 20 "
      "~/Coding/mini-project/tools/bin/tcpputfiles "
      "~/Coding/mini-project/log/idc/tcpputfiles_metdata.log "
      "\"<ip>127.0.0.1</ip>"
      "<port>5005</port>"
      "<ptype>1</ptype>"
      "<clientpath>~/Coding/mini-project/tmp/tcp/metdata1</clientpath>"
      "<andchild>true</andchild>"
      "<matchname>*.XML,*.CSV,*.JSON</matchname>"
      "<srvpath>~/Coding/mini-project/tmp/tcp/metdata2</srvpath>"
      "<timetvl>10</timetvl>"
      "<timeout>50</timeout>"
      "<pname>tcpputfiles_metdata</pname>\"\n");
  printf(
      "~/Coding/mini-project/tools/bin/procctl 20 "
      "~/Coding/mini-project/tools/bin/tcpputfiles "
      "~/Coding/mini-project/log/idc/tcpputfiles_metdata.log "
      "\"<ip>127.0.0.1</ip>"
      "<port>5005</port>"
      "<ptype>2</ptype>"
      "<clientpath>~/Coding/mini-project/tmp/tcp/metdata1</clientpath>"
      "<clientpathbak>~/Coding/mini-project/tmp/tcp/metdata1bak</"
      "clientpathbak>"
      "<andchild>true</andchild>"
      "<matchname>*.XML,*.CSV,*.JSON</matchname>"
      "<srvpath>~/Coding/mini-project/tmp/tcp/metdata2</srvpath>"
      "<timetvl>10</timetvl>"
      "<timeout>50</timeout>"
      "<pname>tcpputfiles_metdata</pname>\"\n\n\n");

  printf("本程序是数据中心的公共功能模块, 采用 TCP 协议把文件上传给服务端.\n");
  printf("logfilename   本程序运行的日志文件.\n");
  printf("xmlbuffer     本程序运行的参数, 如下:\n");
  printf("ip            服务端的 IP 地址.\n");
  printf("port          服务端的端口.\n");
  printf(
      "ptype         "
      "文件上传成功后的处理方式: 1-删除文件; 2-移动到备份目录.\n");
  printf("clientpath    本地文件存放的根目录.\n");
  printf(
      "clientpathbak "
      "文件成功上传后, 本地文件备份的根目录, 当 ptype == 2 时有效.\n");
  printf(
      "andchild      "
      "是否上传 clientpath 目录下各级子目录的文件, "
      "true-是; false-否, 缺省为false.\n");
  printf("matchname     待上传文件名的匹配规则, 如\"*.TXT,*.XML\"\n");
  printf("srvpath       服务端文件存放的根目录.\n");
  printf(
      "timetvl       扫描本地目录文件的时间间隔, 单位: 秒, "
      "取值在 1-30 之间.\n");
  printf(
      "timeout       "
      "本程序的超时时间, 单位: 秒, 视文件大小和网络带宽而定, "
      "建议设置50以上.\n");
  printf(
      "pname         "
      "进程名, 尽可能采用易懂的, 与其它进程不同的名称, 方便故障排查.\n\n");

  return;
}

bool _xmltoarg(char *strxmlbuffer) {
  memset(&starg, 0, sizeof(st_arg));

  GetXMLBuffer(strxmlbuffer, "ip", starg.ip);
  if (strlen(starg.ip) == 0) {
    logfile.Write("ip is null.\n");
    return false;
  }

  GetXMLBuffer(strxmlbuffer, "port", &starg.port);
  if (starg.port == 0) {
    logfile.Write("port is null.\n");
    return false;
  }

  GetXMLBuffer(strxmlbuffer, "ptype", &starg.ptype);
  if (starg.ptype != 1 && starg.ptype != 2) {
    logfile.Write("ptype not in (1, 2).\n");
    return false;
  }

  GetXMLBuffer(strxmlbuffer, "clientpath", starg.clientpath);
  if (strlen(starg.clientpath) == 0) {
    logfile.Write("clientpath is null.\n");
    return false;
  }

  GetXMLBuffer(strxmlbuffer, "clientpathbak", starg.clientpathbak);
  if (starg.ptype == 2 && strlen(starg.clientpathbak) == 0) {
    logfile.Write("clientpathbak is null.\n");
    return false;
  }

  GetXMLBuffer(strxmlbuffer, "andchild", &starg.andchild);

  GetXMLBuffer(strxmlbuffer, "matchname", starg.matchname);
  if (strlen(starg.matchname) == 0) {
    logfile.Write("matchname is null.\n");
    return false;
  }

  GetXMLBuffer(strxmlbuffer, "srvpath", starg.srvpath);
  if (strlen(starg.srvpath) == 0) {
    logfile.Write("srvpath is null.\n");
    return false;
  }

  // 扫描本地目录文件的时间间隔, 单位: 秒.
  // starg.timetvl 没有必要超过 30 秒.
  GetXMLBuffer(strxmlbuffer, "timetvl", &starg.timetvl);
  if (starg.timetvl == 0) {
    logfile.Write("timetvl is null.\n");
    return false;
  }

  if (starg.timetvl > 30) {
    starg.timetvl = 30;
  }

  // 进程心跳的超时时间, 一定要大于 starg.timetvl, 没有必要小于 50 秒.
  GetXMLBuffer(strxmlbuffer, "timeout", &starg.timeout);
  if (starg.timeout == 0) {
    logfile.Write("timeout is null.\n");
    return false;
  }

  if (starg.timeout < 50) {
    starg.timeout = 50;
  }

  GetXMLBuffer(strxmlbuffer, "pname", starg.pname, 50);
  if (strlen(starg.pname) == 0) {
    logfile.Write("pname is null.\n");
    return false;
  }

  return true;
}

bool Login(const char *argv) {
  memset(strsendbuffer, 0, sizeof(strsendbuffer));
  memset(strrecvbuffer, 0, sizeof(strrecvbuffer));

  SPRINTF(strsendbuffer, sizeof(strsendbuffer), "%s<clienttype>1</clienttype>",
          argv);
  logfile.Write("Send: %s\n", strsendbuffer);

  // 向服务端发送请求报文.
  if (!TcpClient.Write(strsendbuffer)) {
    return false;
  }

  // 接收服务端的回应报文.
  if (!TcpClient.Read(strrecvbuffer, 20)) {
    return false;
  }

  logfile.Write("Receive: %s\n", strrecvbuffer);

  logfile.Write("Login(%s: %d) succeeded.\n", starg.ip, starg.port);

  return true;
}

bool ActiveTest() {
  memset(strsendbuffer, 0, sizeof(strsendbuffer));
  memset(strrecvbuffer, 0, sizeof(strrecvbuffer));

  SPRINTF(strsendbuffer, sizeof(strsendbuffer), "<activetest>ok</activetest>");
  // logfile.Write("Send: %s\n", strsendbuffer);

  // 向服务端发送请求报文.
  if (!TcpClient.Write(strsendbuffer)) {
    return false;
  }

  // 接收服务端的回应报文.
  if (!TcpClient.Read(strrecvbuffer, 20)) {
    return false;
  }

  // logfile.Write("Receive: %s\n", strrecvbuffer);

  return true;
}

bool _tcpputfiles() {
  CDir Dir;

  // 调用 OpenDir() 打开 starg.clientpath 目录.
  if (!Dir.OpenDir(starg.clientpath, starg.matchname, 10000, starg.andchild)) {
    logfile.Write("Dir.OpenDir(%s) failed\n", starg.clientpath);
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

    // logfile.Write("strsendbuffer = %s\n", strsendbuffer);
    if (!TcpClient.Write(strsendbuffer)) {
      logfile.Write("TcpClient.Write() failed.\n");
      return false;
    }

    // 把文件的内容发送给对端.
    logfile.Write("send %s(%d) ...", Dir.m_FullFileName, Dir.m_FileSize);
    if (SendFile(TcpClient.m_connfd, Dir.m_FullFileName, Dir.m_FileSize)) {
      logfile.WriteEx("ok.\n");
      ++delayed;
    } else {
      logfile.WriteEx("failed.\n");
      TcpClient.Close();
      return false;
    }

    PActive.UptATime();

    // 接收对端的确认报文.
    while (delayed > 0) {
      memset(strrecvbuffer, 0, sizeof(strrecvbuffer));
      if (!TcpRead(TcpClient.m_connfd, strrecvbuffer, &buflen, -1)) {
        break;
      }
      // logfile.Write("strrecvbuffer =% s\n", strrecvbuffer);

      // 删除或者转存本地的文件.
      --delayed;
      AckMessage(strrecvbuffer);
    }
  }

  // 继续接收对端的确认报文.
  while (delayed > 0) {
    memset(strrecvbuffer, 0, sizeof(strrecvbuffer));
    if (!TcpRead(TcpClient.m_connfd, strrecvbuffer, &buflen, 10)) {
      break;
    }
    // logfile.Write("strrecvbuffer = %s\n", strrecvbuffer);

    // 删除或者转存本地的文件.
    --delayed;
    AckMessage(strrecvbuffer);
  }

  return true;
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

  // 如果服务端接收文件不成功, 直接返回.
  if (strcmp(result, "ok") != 0) {
    return true;
  }

  // ptype == 1, 删除文件.
  if (starg.ptype == 1) {
    if (!REMOVE(filename)) {
      logfile.Write("REMOVE(%s) failed.\n", filename);
      return false;
    }
  }

  // ptype == 2, 移动到备份目录.
  if (starg.ptype == 2) {
    // 生成转存后的备份目录文件名.
    char bakfilename[301];
    STRCPY(bakfilename, sizeof(bakfilename), filename);
    UpdateStr(bakfilename, starg.clientpath, starg.clientpathbak, false);
    if (!RENAME(filename, bakfilename)) {
      logfile.Write("RENAME(%s, %s) failed.\n", filename, bakfilename);
      return false;
    }
  }

  return true;
}
