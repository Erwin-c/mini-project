/*
 * execsql.cc, 这是一个工具程序, 用于执行一个 SQL 脚本文件.
 *
 * Author: Erwin
 */

#include "_mysql.h"
#include "_public.h"

CLogFile logfile;  // 日志类.

CPActive PActive;  // 进程心跳类.

connection conn;  // 数据库连接类.

// 程序帮助文档.
void _help();

// 程序退出和信号 2, 15 的处理函数.
void EXIT(int sig);

int main(int argc, char *argv[]) {
  // 帮助文档。
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

  PActive.AddPInfo(500, "execsql");  // 进程的心跳，时间长的点。
  // 注意，在调试程序的时候，可以启用类似以下的代码，防止超时。
  // PActive.AddPInfo(5000,"obtcodetodb");

  // 连接数据库, 不启用事务.
  if (conn.connecttodb(argv[2], argv[3], 1) != 0) {
    logfile.Write("connect database(%s) 失败.\n%s\n", argv[2],
                  conn.m_cda.message);
    return -1;
  }

  logfile.Write("connect database(%s) 成功.\n", argv[2]);

  CFile File;

  // 打开 SQL 文件.
  if (!File.Open(argv[1], "r")) {
    logfile.Write("File.Open(%s) 失败.\n", argv[1]);
    EXIT(-1);
  }

  char strsql[1001];  // 存放从 SQL 文件中读取的 SQL 语句.

  while (true) {
    memset(strsql, 0, sizeof(strsql));

    // 从 SQL 文件中读取以分号结束的一行.
    if (!File.FFGETS(strsql, 1000, ";")) {
      break;
    }

    // 如果第一个字符是#, 注释, 不执行.
    if (strsql[0] == '#') {
      continue;
    }

    // 删除掉 SQL 语句最后的分号.
    char *pp = strstr(strsql, ";");
    if (pp == nullptr) {
      continue;
    }
    pp[0] = 0;

    logfile.Write("%s\n", strsql);

    // 执行 SQL 语句.
    int iret = conn.execute(strsql);

    // 把 SQL 语句执行结果写日志.
    if (iret == 0) {
      logfile.Write("exec 成功 (rpc = %d).\n", conn.m_cda.rpc);
    } else {
      logfile.Write("exec 失败 (%s).\n", conn.m_cda.message);
    }

    // 进程的心跳.
    PActive.UptATime();
  }

  logfile.WriteEx("\n");

  return 0;
}

void _help() {
  printf("\n");
  printf("Using:./execsql sqlfile connstr charset logfile\n");

  printf(
      "Example: ~/Coding/mini-project/tools/bin/procctl 120 "
      "~/Coding/mini-project/tools/bin/execsql "
      "~/Coding/mini-project/idc/sql/cleardata.sql "
      "\"127.0.0.1,root,rooterwin,mysql,3306\" utf8 "
      "~/Coding/mini-project/log/idc/execsql.log\n\n");
  printf(
      "        ~/Coding/mini-project/tools/bin/execsql "
      "~/Coding/mini-project/idc/sql/cleardata.sql "
      "\"127.0.0.1,root,rooterwin,mysql,3306\" utf8 "
      "~/Coding/mini-project/log/idc/execsql.log\n\n");

  printf("这是一个工具程序, 用于执行一个 SQL 脚本文件.\n");
  printf(
      "sqlfile "
      "SQL 脚本文件名, 每条 SQL 语句可以多行书写, 分号表示一条sql语句的结束,"
      "不支持注释.\n");
  printf("connstr 数据库连接参数: ip,username,password,dbname,port.\n");
  printf("charset 数据库的字符集.\n");
  printf("logfile 本程序运行的日志文件名.\n");
  printf("程序每 120 秒运行一次, 由 procctl 调度.\n\n\n");

  return;
}

void EXIT(int sig) {
  logfile.Write("程序退出, sig = %d\n", sig);

  conn.disconnect();

  exit(0);
}
