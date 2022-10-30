/*
 * demo10.cc, 本程序演示线程和信号.
 *
 *  Author: Erwin
 */

#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void func(int sig);

// 线程主函数.
void *thmain(void *arg);

int main() {
  signal(2, func);

  // 创建线程.
  pthread_t thid;
  if (pthread_create(&thid, NULL, thmain, NULL) != 0) {
    printf("pthread_create() failed.\n");
    exit(-1);
  }

  sleep(5);
  pthread_kill(thid, 15);
  sleep(100);

  printf("Join ...\n");
  pthread_join(thid, NULL);
  printf("Join ok.\n");

  return 0;
}

void func(int sig) {
  printf("func() catch signal %d\n", sig);
  return;
}

void *thmain(void *arg) {
  printf("Sleep ....\n");
  sleep(10);
  printf("Sleep ok.\n");

  return (void *)1;
}
