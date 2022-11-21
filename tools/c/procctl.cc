/*
 * procctl.cc, 本程序是服务程序的调度程序.
 *
 *  Author: Erwin
 */

#include "_public.h"

int main(int argc, char *argv[]) {
  if (argc < 3) {
    printf("Using: ./procctl timetvl program argv ...\n");
    printf(
        "Example: ~Coding/mini-project/tools/bin/procctl 5 "
        "~/Coding/mini-project/idc/bin/crtmetdata "
        "~/Coding/mini-project/idc/ini/stcode.ini "
        "~/Coding/mini-project/tmp/metdata "
        "~/Coding/mini-project/log/crtmetdata.log "
        "xml,json,csv\n\n");

    printf("本程序是服务程序的调度程序, 周期性启动服务程序或 shell 脚本.\n");
    printf(
        "timetvl 运行周期, 单位: 秒. "
        "被调度的程序运行结束后, 在 timetvl 秒后会被 procctl 重新启动.\n");
    printf("program 被调度的程序名, 必须使用全路径.\n");
    printf("argvs   被调度的程序的参数.\n");

    printf("注意, 本程序不会被 kill 杀死, 但可以用 kill -9 强行杀死.\n\n\n");

    return -1;
  }

  // 关闭信号和 IO, 本程序不希望被打扰.
  CloseIOAndSignal(true);

  // 生成子进程, 父进程退出, 让程序运行在后台, 由系统 1 号进程托管.
  if (fork() != 0) {
    exit(0);
  }

  // 启用 SIGCHLD 信号, 让父进程可以 wait 子进程退出的状态.
  signal(SIGCHLD, SIG_DFL);

  char *pargv[argc];
  for (int ii = 2; ii < argc; ++ii) {
    pargv[ii - 2] = argv[ii];
  }

  pargv[argc - 2] = nullptr;

  while (true) {
    if (fork() == 0) {
      execv(argv[2], pargv);
      exit(0);
    } else {
      int status;
      wait(&status);
      sleep(atoi(argv[1]));
    }
  }

  return 0;
}
