/*
 * obtmindtodb.cc, 本程序用于把全国站点分钟观测数据入库到 T_ZHOBTMIND 表中,
 *                 支持 xml 和 csv 两种文件格式.
 *
 * Author: Erwin
 */

#include "idcapp.h"

CLogFile logfile;  // 日志类.

CPActive PActive;  // 进程心跳类.

connection conn;  // 数据库连接类.

// 业务处理主函数.
bool _obtmindtodb(char *pathname, char *connstr, char *charset);

// 程序帮助文档.
void _help();

// 程序退出和信号 2, 15 的处理函数.
void EXIT(int sig);

int main(int argc, char *argv[]) {
  // 帮助文档.
  if (argc != 5) {
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
  if (!logfile.Open(argv[4], "a+")) {
    printf("打开日志文件(%s) 失败.\n", argv[4]);
    return -1;
  }

  // 进程的心跳, 30 秒足够.
  PActive.AddPInfo(30, "obtmindtodb");
  // 注意, 在调试程序的时候, 可以启用类似以下的代码, 防止超时.
  // PActive.AddPInfo(5000, "obtmindtodb");

  // 业务处理主函数.
  _obtmindtodb(argv[1], argv[2], argv[3]);

  return 0;
}

bool _obtmindtodb(char *pathname, char *connstr, char *charset) {
  CDir Dir;

  // 打开目录.
  if (!Dir.OpenDir(pathname, "*.xml")) {
    logfile.Write("Dir.OpenDir(%s) 失败.\n", pathname);
    return false;
  }

  CFile File;

  CZHOBTMIND ZHOBTMIND(&conn, &logfile);

  int totalcount = 0;   // 文件的总记录数.
  int insertcount = 0;  // 成功插入记录数.
  bool bisxml = false;  // true-xml, false-csv.
  CTimer Timer;         // 计时器, 记录每个数据文件的处理耗时.

  while (true) {
    // 读取目录, 得到一个数据文件名.
    if (!Dir.ReadDir()) {
      break;
    }

    bisxml = MatchStr(Dir.m_FullFileName, "*.xml") ? true : false;

    // 连接数据库.
    if (conn.m_state == 0) {
      if (conn.connecttodb(connstr, charset) != 0) {
        logfile.Write("connect database(%s) 失败.\n%s\n", connstr,
                      conn.m_cda.message);
        return -1;
      }

      logfile.Write("connect database(%s) 成功.\n", connstr);
    }

    totalcount = insertcount = 0;

    // 打开文件.
    if (!File.Open(Dir.m_FullFileName, "r")) {
      logfile.Write("File.Open(%s) 失败.\n", Dir.m_FullFileName);
      return false;
    }

    char strBuffer[1001];  // 存放从文件中读取的一行.

    while (true) {
      if (bisxml) {
        if (!File.FFGETS(strBuffer, 1000, "<endl/>")) {
          break;
        }
      } else {
        if (!File.Fgets(strBuffer, 1000, true)) {
          break;
        }
        if (strstr(strBuffer, "站点") != 0) {
          continue;
        }
      }

      // 处理文件中的每一行.
      ++totalcount;

      ZHOBTMIND.SplitBuffer(strBuffer, bisxml);

      if (ZHOBTMIND.InsertTable()) {
        ++insertcount;
      }
    }

    // 删除文件, 提交事务.
    // File.CloseAndRemove();

    conn.commit();

    logfile.Write(
        "已处理文件 %s (totalcount = %d, insertcount = %d)m 耗时 %.2f 秒.\n",
        Dir.m_FullFileName, totalcount, insertcount, Timer.Elapsed());
  }

  return true;
}

void _help() {
  printf("\n");
  printf("Using:./obtmindtodb pathname connstr charset logfile\n");

  printf(
      "Example: /home/erwin/Coding/mini-project/tools/bin/procctl 10 "
      "/home/erwin/Coding/mini-project/idc/bin/obtmindtodb "
      "/home/erwin/Coding/mini-project/idcdata/surfdata "
      "\"127.0.0.1,root,rooterwin,mysql,3306\" utf8 "
      "/home/erwin/Coding/mini-project/log/idc/obtmindtodb.log\n");
  printf(
      "        /home/erwin/Coding/mini-project/idc/bin/obtmindtodb "
      "/home/erwin/Coding/mini-project/idcdata/surfdata "
      "\"127.0.0.1,root,rooterwin,mysql,3306\" utf8 "
      "/home/erwin/Coding/mini-project/log/idc/obtmindtodb.log\n\n");

  printf(
      "本程序用于把全国站点分钟观测数据保存到数据库的 T_ZHOBTMIND 表中,"
      "数据只插入, 不更新.\n");
  printf("pathname 全国站点分钟观测数据文件存放的目录.\n");
  printf("connstr  数据库连接参数: ip,username,password,dbname,port.\n");
  printf("charset  数据库的字符集.\n");
  printf("logfile  本程序运行的日志文件名.\n");
  printf("程序每 10 秒运行一次, 由 procctl 调度.\n\n\n");

  return;
}

void EXIT(int sig) {
  logfile.Write("程序退出, sig = %d\n", sig);

  conn.disconnect();

  exit(0);
}
