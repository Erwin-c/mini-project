/*
 * tcpgetfiles.cc, 采用 TCP 协议, 实现文件下载的客户端.
 *
 *  Author: Erwin
 */

#include "_public.h"

// 程序运行的参数结构体.
struct st_arg {
  int clienttype;     // 客户端类型, 1-上传文件; 2-下载文件.
  char ip[31];        // 服务端的 IP 地址.
  int port;           // 服务端的端口.
  int ptype;          // 文件下载成功后服务端文件的处理方式:
                      // 1-删除文件; 2-移动到备份目录.
  char srvpath[301];  // 服务端文件存放的根目录.
  char srvpathbak[301];  // 文件成功下载后, 服务端文件备份的根目录,
                         // 当 ptype == 2 时有效.
  bool andchild;  // 是否下载 srvpath 目录下各级子目录的文件, true-是; false-否.
  char matchname[301];  // 待下载文件名的匹配规则, 如 "*.TXT, *.XML".
  char clientpath[301];  // 客户端文件存放的根目录.
  int timetvl;     // 扫描服务端目录文件的时间间隔, 单位: 秒.
  int timeout;     // 进程心跳的超时时间.
  char pname[51];  // 进程名, 建议用 "tcpgetfiles_后缀" 的方式.
} starg;

char strrecvbuffer[1024];  // 发送报文的 buffer.

char strsendbuffer[1024];  // 接收报文的 buffer.

CLogFile logfile;  // 日志类.

CTcpClient TcpClient;  // Socket 通讯的客户端类.

CPActive PActive;  // 进程心跳。

// 登录业务.
bool Login(const char *argv);

// 接收文件的内容.
bool RecvFile(const int sockfd, const char *filename, const char *mtime,
              int filesize);

// 文件下载的主函数.
void _tcpgetfiles();

// 程序帮助文档.
void _help();

// 把xml解析到参数 starg 结构中.
bool _xmltoarg(char *strxmlbuffer);

// 程序退出和信号 2, 15 的处理函数.
void EXIT(int sig);

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
    printf("logfile.Open(%s) 失败.\n", argv[1]);
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
    logfile.Write("TcpClient.ConnectToServer(%s, %d) 失败.\n", starg.ip,
                  starg.port);
    EXIT(-1);
  }

  // 登录业务.
  if (!Login(argv[2])) {
    logfile.Write("Login() 失败.\n");
    EXIT(-1);
  }

  // 调用文件下载的主函数.
  _tcpgetfiles();

  EXIT(0);
}

bool Login(const char *argv) {
  memset(strsendbuffer, 0, sizeof(strsendbuffer));
  memset(strrecvbuffer, 0, sizeof(strrecvbuffer));

  SPRINTF(strsendbuffer, sizeof(strsendbuffer), "%s<clienttype>2</clienttype>",
          argv);
  logfile.Write("发送: %s\n", strsendbuffer);

  // 向服务端发送请求报文.
  if (!TcpClient.Write(strsendbuffer)) {
    return false;
  }

  // 接收服务端的回应报文.
  if (!TcpClient.Read(strrecvbuffer, 20)) {
    return false;
  }

  logfile.Write("接收: %s\n", strrecvbuffer);

  logfile.Write("登录 (%s : %d) 成功.\n", starg.ip, starg.port);

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
  FILE *fp = nullptr;

  // 创建临时文件。
  if ((fp = FOPEN(strfilenametmp, "wb")) == nullptr) {
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

  // 把临时文件 RENAME 为正式的文件.
  if (!RENAME(strfilenametmp, filename)) {
    return false;
  }

  return true;
}

void _tcpgetfiles() {
  PActive.AddPInfo(starg.timeout, starg.pname);

  while (true) {
    memset(strsendbuffer, 0, sizeof(strsendbuffer));
    memset(strrecvbuffer, 0, sizeof(strrecvbuffer));

    PActive.UptATime();

    // 接收服务端的报文.
    // 第二个参数的取值必须大于 starg.timetvl, 小于 starg.timeout.
    if (!TcpClient.Read(strrecvbuffer, starg.timetvl + 10)) {
      logfile.Write("TcpClient.Read() 失败.\n");
      return;
    }
    logfile.Write("strrecvbuffer = %s\n", strrecvbuffer);

    // 处理心跳报文.
    if (strcmp(strrecvbuffer, "<activetest>ok</activetest>") == 0) {
      strcpy(strsendbuffer, "ok");
      logfile.Write("strsendbuffer = %s\n", strsendbuffer);
      if (!TcpClient.Write(strsendbuffer)) {
        logfile.Write("TcpClient.Write() 失败.\n");
        return;
      }
    }

    // 处理下载文件的请求报文.
    if (strncmp(strrecvbuffer, "<filename>", 10) == 0) {
      // 解析下载文件请求报文的 XML.
      char serverfilename[301];
      memset(serverfilename, 0, sizeof(serverfilename));
      char mtime[21];
      memset(mtime, 0, sizeof(mtime));
      int filesize = 0;
      GetXMLBuffer(strrecvbuffer, "filename", serverfilename, 300);
      GetXMLBuffer(strrecvbuffer, "mtime", mtime, 19);
      GetXMLBuffer(strrecvbuffer, "size", &filesize);

      // 客户端和服务端文件的目录是不一样的, 以下代码生成客户端的文件名.
      // 把文件名中的 srvpath 替换成 clientpath, 要小心第三个参数.
      char clientfilename[301];
      memset(clientfilename, 0, sizeof(clientfilename));
      strcpy(clientfilename, serverfilename);
      UpdateStr(clientfilename, starg.srvpath, starg.clientpath, false);

      // 接收文件的内容.
      logfile.Write("接收 %s(%d) ...", clientfilename, filesize);
      if (RecvFile(TcpClient.m_connfd, clientfilename, mtime, filesize)) {
        logfile.WriteEx("成功.\n");
        SNPRINTF(strsendbuffer, sizeof(strsendbuffer), 1000,
                 "<filename>%s</filename><result>ok</result>", serverfilename);
      } else {
        logfile.WriteEx("失败.\n");
        SNPRINTF(strsendbuffer, sizeof(strsendbuffer), 1000,
                 "<filename>%s</filename><result>failed</result>",
                 serverfilename);
      }

      // 把接收结果返回给对端.
      logfile.Write("strsendbuffer = %s\n", strsendbuffer);
      if (!TcpClient.Write(strsendbuffer)) {
        logfile.Write("TcpClient.Write() 失败.\n");
        return;
      }
    }
  }

  return;
}

void _help() {
  printf("\n");

  printf(
      "Using: /home/erwin/Coding/mini-project/tools/bin/tcpgetfiles "
      "logfilename "
      "xmlbuffer\n\n");

  printf(
      "Example: /home/erwin/Coding/mini-project/tools/bin/procctl 20 "
      "/home/erwin/Coding/mini-project/tools/bin/tcpgetfiles "
      "/home/erwin/Coding/mini-project/log/idc/tcpgetfiles_metdata.log "
      "\"<ip>127.0.0.1</ip>"
      "<port>5005</port>"
      "<ptype>1</ptype>"
      "<srvpath>/home/erwin/Coding/mini-project/tmp/server</srvpath>"
      "<andchild>true</andchild>"
      "<matchname>*.XML,*.CSV,*.JSON</matchname>"
      "<clientpath>/home/erwin/Coding/mini-project/tmp/client</"
      "clientpath>"
      "<timetvl>10</timetvl>"
      "<timeout>50</timeout>"
      "<pname>tcpgetfiles_metdata</pname>\"\n");
  printf(
      "         /home/erwin/Coding/mini-project/tools/bin/procctl 20 "
      "/home/erwin/Coding/mini-project/tools/bin/tcpgetfiles "
      "/home/erwin/Coding/mini-project/log/idc/tcpgetfiles_metdata.log "
      "\"<ip>127.0.0.1</ip>"
      "<port>5005</port>"
      "<ptype>2</ptype>"
      "<srvpath>/home/erwin/Coding/mini-project/tmp/server</srvpath>"
      "<srvpathbak>/home/erwin/Coding/mini-project/tmp/serverbak</"
      "srvpathbak>"
      "<andchild>true</andchild>"
      "<matchname>*.XML,*.CSV,*.JSON</matchname>"
      "<clientpath>/home/erwin/Coding/mini-project/tmp/client</"
      "clientpath>"
      "<timetvl>10</timetvl>"
      "<timeout>50</timeout>"
      "<pname>tcpgetfiles_metdata</pname>\"\n");
  printf(
      "         /home/erwin/Coding/mini-project/tools/bin/tcpgetfiles "
      "/home/erwin/Coding/mini-project/log/idc/tcpgetfiles_metdata.log "
      "\"<ip>127.0.0.1</ip>"
      "<port>5005</port>"
      "<ptype>1</ptype>"
      "<srvpath>/home/erwin/Coding/mini-project/tmp/server</srvpath>"
      "<andchild>true</andchild>"
      "<matchname>*.XML,*.CSV,*.JSON</matchname>"
      "<clientpath>/home/erwin/Coding/mini-project/tmp/client</"
      "clientpath>"
      "<timetvl>10</timetvl>"
      "<timeout>50</timeout>"
      "<pname>tcpgetfiles_metdata</pname>\"\n");
  printf(
      "         /home/erwin/Coding/mini-project/tools/bin/tcpgetfiles "
      "/home/erwin/Coding/mini-project/log/idc/tcpgetfiles_metdata.log "
      "\"<ip>127.0.0.1</ip>"
      "<port>5005</port>"
      "<ptype>2</ptype>"
      "<srvpath>/home/erwin/Coding/mini-project/tmp/server</srvpath>"
      "<srvpathbak>/home/erwin/Coding/mini-project/tmp/serverbak</"
      "srvpathbak>"
      "<andchild>true</andchild>"
      "<matchname>*.XML,*.CSV,*.JSON</matchname>"
      "<clientpath>/home/erwin/Coding/mini-project/tmp/client</"
      "clientpath>"
      "<timetvl>10</timetvl>"
      "<timeout>50</timeout>"
      "<pname>tcpgetfiles_metdata</pname>\"\n\n\n");

  printf("本程序是数据中心的公共功能模块, 采用 TCP 协议从服务端下载文件.\n");
  printf("logfilename   本程序运行的日志文件.\n");
  printf("xmlbuffer     本程序运行的参数, 如下:\n");
  printf("ip            服务端的 IP 地址.\n");
  printf("port          服务端的端口.\n");
  printf(
      "ptype         "
      "文件下载成功后服务端文件的处理方式: 1-删除文件; 2-移动到备份目录.\n");
  printf("srvpath       服务端文件存放的根目录.\n");
  printf(
      "srvpathbak    "
      "文件成功下载后, 服务端文件备份的根目录, 当 ptype == 2 时有效.\n");
  printf(
      "andchild      "
      "是否下载 srvpath 目录下各级子目录的文件, true-是; false-"
      "否, 缺省为 false.\n");
  printf("matchname     待下载文件名的匹配规则, 如\"*.TXT,*.XML\"\n");
  printf("clientpath    客户端文件存放的根目录.\n");
  printf(
      "timetvl       扫描服务目录文件的时间间隔, 单位: 秒, 取值在 1-30 "
      "之间.\n");
  printf(
      "timeout       "
      "本程序的超时时间, 单位: 秒, 视文件大小和网络带宽而定, 建议设置 50 以上."
      "\n");
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

  GetXMLBuffer(strxmlbuffer, "srvpath", starg.srvpath);
  if (strlen(starg.srvpath) == 0) {
    logfile.Write("srvpath is null.\n");
    return false;
  }

  GetXMLBuffer(strxmlbuffer, "srvpathbak", starg.srvpathbak);
  if (starg.ptype == 2 && strlen(starg.srvpathbak) == 0) {
    logfile.Write("srvpathbak is null.\n");
    return false;
  }

  GetXMLBuffer(strxmlbuffer, "andchild", &starg.andchild);

  GetXMLBuffer(strxmlbuffer, "matchname", starg.matchname);
  if (strlen(starg.matchname) == 0) {
    logfile.Write("matchname is null.\n");
    return false;
  }

  GetXMLBuffer(strxmlbuffer, "clientpath", starg.clientpath);
  if (strlen(starg.clientpath) == 0) {
    logfile.Write("clientpath is null.\n");
    return false;
  }

  GetXMLBuffer(strxmlbuffer, "timetvl", &starg.timetvl);
  if (starg.timetvl == 0) {
    logfile.Write("timetvl is null.\n");
    return false;
  }

  // 扫描服务端目录文件的时间间隔, 单位: 秒.
  // starg.timetvl 没有必要超过 30 秒.
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

void EXIT(int sig) {
  logfile.Write("程序退出, sig = %d\n\n", sig);

  exit(0);
}
