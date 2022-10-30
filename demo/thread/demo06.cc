/*
 * demo06.cc, 本程序演示线程资源的回收, 用 pthread_join() 非分离的线程.
 *
 *  Author: Erwin
 */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// 线程主函数.
void *thmain1(void *arg);
// 线程主函数.
void *thmain2(void *arg);

int main() {
  pthread_t thid1, thid2;

  // 创建线程.
  if (pthread_create(&thid1, NULL, thmain1, NULL) != 0) {
    printf("pthread_create() failed.\n");
    exit(-1);
  }

  if (pthread_create(&thid2, NULL, thmain2, NULL) != 0) {
    printf("pthread_create() failed.\n");
    exit(-1);
  }

  sleep(10);

  void *ret;
  printf("Join...\n");

  int result = 0;
  result = pthread_join(thid2, &ret);
  printf("thid2 result = %d, ret = %d\n", result, (int)(long)ret);
  result = pthread_join(thid1, &ret);
  printf("thid1 result = %d, ret= %d\n", result, (int)(long)ret);

  ret = NULL;
  result = pthread_join(thid2, &ret);
  printf("thid2 result = %d,ret = %d\n", result, (int)(long)ret);
  result = pthread_join(thid1, &ret);
  printf("thid1 result = %d,ret = %d\n", result, (int)(long)ret);

  printf("Join ok.\n");

  return 0;
}

void *thmain1(void *arg) {
  for (int ii = 0; ii < 3; ++ii) {
    sleep(1);
    printf("pthmain1() sleep(%d) ok.\n", ii + 1);
  }

  return (void *)1;
}

void *thmain2(void *arg) {
  for (int ii = 0; ii < 5; ++ii) {
    sleep(1);
    printf("pthmain2() sleep(%d) ok.\n", ii + 1);
  }

  return (void *)2;
}
