/*
 * checkproc.cc, 本程序用于检查后台服务程序是否超时.
 *
 *  Author: Erwin
 */

#include "_public.h"

CLogFile logfile;  // 程序运行的日志.

int main(int argc, char *argv[]) {
  // 程序的帮助.
  if (argc != 2) {
    printf("\n");
    printf("Using: ./checkproc logfilename\n");

    printf(
        "Example: /home/erwin/Coding/mini-project/tools/bin/procctl 10 "
        "/home/erwin/Coding/mini-project/tools/bin/checkproc "
        "~Coding/mini-project/log/checkproc.log\n\n");

    printf("本程序用于检查后台服务程序是否超时, 如果已超时, 就终止它.\n");
    printf("注意: \n");
    printf("1) 本程序由 procctl 启动, 运行周期建议为 10 秒.\n");
    printf("2) 为了避免被普通用户误杀, 本程序应该用 root 用户启动.\n");
    printf("3) 如果要停止本程序, 只能用 killall -9 终止.\n\n\n");

    return 0;
  }

  // 忽略全部的信号和 IO, 不希望程序被干扰.
  CloseIOAndSignal(true);

  // 打开日志文件.
  if (logfile.Open(argv[1], "a+") == false) {
    printf("logfile.Open(%s) failed.\n", argv[1]);
    return -1;
  }

  int shmid = 0;

  // 创建/获取共享内存, 键值为 SHMKEYP, 大小为 MAXNUMP 个 st_procinfo
  // 结构体的大小.
  if ((shmid = shmget((key_t)SHMKEYP, MAXNUMP * sizeof(st_procinfo),
                      0666 | IPC_CREAT)) == -1) {
    logfile.Write("创建/获取共享内存 (%x) 失败.\n", SHMKEYP);
    return false;
  }

  // 将共享内存连接到当前进程的地址空间.
  st_procinfo *shm = (st_procinfo *)shmat(shmid, 0, 0);

  // 遍历共享内存中全部的记录.
  for (int ii = 0; ii < MAXNUMP; ++ii) {
    // 如果记录的 pid == 0, 表示空记录, continue.
    if (shm[ii].pid == 0) {
      continue;
    }

    // 如果记录的 pid != 0, 表示是服务程序的心跳记录.

    // 向进程发送信号 0, 判断它是否还存在, 如果不存在, 从共享内存中删除该记录,
    // continue.
    int iret = kill(shm[ii].pid, 0);
    if (iret == -1) {
      logfile.Write("进程 pid = %d (%s) 已经不存在.\n", (shm + ii)->pid,
                    (shm + ii)->pname);
      // 从共享内存中删除该记录.
      memset(shm + ii, 0, sizeof(st_procinfo));
      continue;
    }

    time_t now = time(0);  // 取当前时间.

    // 如果进程未超时, continue.
    if (now - shm[ii].atime < shm[ii].timeout) {
      continue;
    }

    // 如果已超时。
    logfile.Write("进程 pid = %d (%s) 已经超时.\n", (shm + ii)->pid,
                  (shm + ii)->pname);

    // 发送信号 15, 尝试正常终止进程.
    kill(shm[ii].pid, 15);

    // 每隔 1 秒判断一次进程是否存在, 累计 5 秒, 一般来说,
    // 5 秒的时间足够让进程退出.
    for (int jj = 0; jj < 5; ++jj) {
      sleep(1);
      // 向进程发送信号 0, 判断它是否还存在.
      iret = kill(shm[ii].pid, 0);
      // 进程已退出.
      if (iret == -1) {
        break;
      }
    }

    if (iret == -1) {
      logfile.Write("进程 pid = %d (%s) 已经正常终止.\n", (shm + ii)->pid,
                    (shm + ii)->pname);
    } else {
      // 如果进程仍存在, 就发送信号 9, 强制终止它.
      kill(shm[ii].pid, 9);
      logfile.Write("进程 pid = %d (%s) 已经强制终止.\n", (shm + ii)->pid,
                    (shm + ii)->pname);
    }

    // 从共享内存中删除已超时进程的心跳记录.
    // 从共享内存中删除该记录。
    memset(shm + ii, 0, sizeof(st_procinfo));
  }

  // 把共享内存从当前进程中分离.
  shmdt(shm);

  return 0;
}
