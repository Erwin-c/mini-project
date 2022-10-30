/*
 * demo09.cc, 本程序演示线程的取消.
 *
 *  Author: Erwin
 */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int var = 0;

// 线程主函数.
void *thmain(void *arg);

int main() {
  pthread_t thid;

  // 创建线程.
  if (pthread_create(&thid, NULL, thmain, NULL) != 0) {
    printf("pthread_create() failed.\n");
    exit(-1);
  }

  usleep(100);
  pthread_cancel(thid);

  int result;
  void *ret;

  printf("Join...\n");
  result = pthread_join(thid, &ret);
  printf("thid result = %d, ret = %d\n", result, (int)(long)ret);
  printf("Join ok.\n");

  printf("var = %d\n", var);

  return 0;
}

void *thmain(void *arg) {
  // pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

  for (var = 0; var < 400000000; ++var) {
    ;
    pthread_testcancel();
  }

  return (void *)1;
}
