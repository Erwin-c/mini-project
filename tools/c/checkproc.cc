/*
 * checkproc.cc
 *
 *  Author: Erwin
 */

#include <_public.h>
#include <signal.h>
#include <sys/shm.h>
#include <unistd.h>

#include <cstring>

CLogFile logfile;

int main(int argc, char *argv[]) {
  if (argc != 2) {
    printf("Using:./checkproc logfilename\n");
    printf(
        "Example:../bin/procctl 10 ../bin/checkproc "
        "../../tmp/log/checkproc.log\n");

    printf(
        "This program is used to check whether the background service program "
        "has timed out. If it has timed out, terminate it.\n");

    printf("Note: \n");
    printf(
        "  1) This program is started by procctl, running cycle is 10 "
        "seconds\n");
    printf(
        "  2) This program should be started by root user to avoid being "
        "killed users\n");
    printf("  3) You can only use 'kill - 9' to stop this program\n");

    return 0;
  }

  CloseIOAndSignal(true);

  if (!logfile.Open(argv[1], "a+")) {
    printf("logfile.Open(%s) failed\n", argv[1]);
    return -1;
  }

  int shmid = 0;

  // 创建/获取共享内存，键值为SHMKEYP，大小为MAXNUMP个st_procinfo结构体的大小。
  if ((shmid = shmget((key_t)SHMKEYP, MAXNUMP * sizeof(struct st_procinfo),
                      0666 | IPC_CREAT)) == -1) {
    logfile.Write("Create/Get semaphore(%x) falied\n", SHMKEYP);
    return false;
  }

  struct st_procinfo *shm = (struct st_procinfo *)shmat(shmid, 0, 0);

  for (int ii = 0; ii < MAXNUMP; ++ii) {
    if (shm[ii].pid == 0) {
      continue;
    }

    // logfile.Write("ii=%d, pid=%d, pname=%s, timeout=%d, atime=%d\n",\ii,
    //               shm[ii].pid, shm[ii].pname, shm[ii].timeout,
    //               shm[ii].atime);

    int iret = kill(shm[ii].pid, 0);
    if (iret == -1) {
      logfile.Write("Process pid=%d(%s) does not exist\n", (shm + ii)->pid,
                    (shm + ii)->pname);
      memset(shm + ii, 0, sizeof(struct st_procinfo));
      continue;
    }

    time_t now = time(0);

    if (now - shm[ii].atime < shm[ii].timeout) {
      continue;
    }

    logfile.Write("Process pid=%d(%s) has expired\n", (shm + ii)->pid,
                  (shm + ii)->pname);

    kill(shm[ii].pid, 15);

    for (int jj = 0; jj < 5; ++jj) {
      sleep(1);
      iret = kill(shm[ii].pid, 0);
      if (iret == -1) {
        break;
      }
    }

    if (iret == -1) {
      logfile.Write("Process pid=%d(%s) has been terminated\n", (shm + ii)->pid,
                    (shm + ii)->pname);
    } else {
      kill(shm[ii].pid, 9);
      logfile.Write("Process pid=%d(%s) has been terminated by kill '9'\n",
                    (shm + ii)->pid, (shm + ii)->pname);
    }

    memset(shm + ii, 0, sizeof(struct st_procinfo));
  }

  shmdt(shm);

  return 0;
}
