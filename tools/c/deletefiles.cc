/*
 * deletefiles.cc, 本程序用于删除历史的数据文件或日志文件.
 *
 *  Author: Erwin
 */

#include "_public.h"

// 程序帮助文档.
void _help();

// 程序退出和信号 2, 15 的处理函数.
void EXIT(int sig);

int main(int argc, char *argv[]) {
  // 程序的帮助.
  if (argc != 4) {
    _help();
    return -1;
  }

  // 关闭全部的信号和输入输出.
  // 设置信号, 在 shell 状态下可用 "kill + 进程号" 正常终止些进程.
  // 但请不要用 "kill -9 + 进程号" 强行终止.
  CloseIOAndSignal(true);

  signal(SIGINT, EXIT);
  signal(SIGTERM, EXIT);

  // 获取文件超时的时间点.
  char strTimeOut[21];
  LocalTime(strTimeOut, "yyyy-mm-dd hh24:mi:ss",
            0 - (int)(atof(argv[3]) * 24 * 60 * 60));

  CDir Dir;
  // 打开目录, CDir.OpenDir().
  if (!Dir.OpenDir(argv[1], argv[2], 10000, true)) {
    printf("Dir.OpenDir(%s) 失败.\n", argv[1]);
    return -1;
  }

  // 遍历目录中的文件名.
  while (true) {
    // 得到一个文件的信息, CDir.ReadDir().
    if (!Dir.ReadDir()) {
      break;
    }
    printf("=%s=\n", Dir.m_FullFileName);
    // 与超时的时间点比较, 如果更早, 就需要删除.
    if (strcmp(Dir.m_ModifyTime, strTimeOut) < 0) {
      if (REMOVE(Dir.m_FullFileName) == 0) {
        printf("REMOVE %s 成功.\n", Dir.m_FullFileName);
      } else {
        printf("REMOVE %s 失败.\n", Dir.m_FullFileName);
      }
    }
  }

  return 0;
}

void _help() {
  printf("\n");

  printf(
      "Using: /home/erwin/Coding/mini-project/tools/bin/deletefiles pathname "
      "matchstr "
      "timeout\n\n");
  printf(
      "Example: /home/erwin/Coding/mini-project/tools/bin/procctl 300 "
      "/home/erwin/Coding/mini-project/tools/bin/deletefiles "
      "/home/erwin/Coding/mini-project/log/idc \"*.log*\" 0.02\n");
  printf(
      "         /home/erwin/Coding/mini-project/tools/bin/procctl 300 "
      "/home/erwin/Coding/mini-project/tools/bin/deletefiles "
      "/home/erwin/Coding/mini-project/idcdata/surfdata \"*.xml,*.json,*.csv\" "
      "0.01\n");
  printf(
      "         /home/erwin/Coding/mini-project/tools/bin/deletefiles "
      "/home/erwin/Coding/mini-project/log/idc \"*.log*\" 0.02\n");
  printf(
      "         /home/erwin/Coding/mini-project/tools/bin/deletefiles "
      "/home/erwin/Coding/mini-project/idcdata/surfdata \"*.xml,*.json,*.csv\" "
      "0.01\n\n");

  printf("这是一个工具程序, 用于删除历史的数据文件或日志文件.\n");
  printf(
      "本程序把 pathname 目录及子目录中 timeout 天之前的匹配 matchstr "
      "文件全部删除, timeout 可以是小数.\n");
  printf("本程序不写日志文件, 也不会在控制台输出任何信息.");
  printf("程序每 300 秒运行一次, 由 procctl 调度.\n\n\n");

  return;
}

void EXIT(int sig) {
  printf("程序退出, sig = %d\n", sig);

  exit(0);
}
