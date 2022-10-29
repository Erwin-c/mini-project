/*
 * demo03.cc, 本程序演示线程参数的传递(不用强制转换的方法, 传变量的地址).
 *
 *  Author: Erwin
 */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int var;

// 线程 1 的主函数.
void *thmain1(void *arg);
// 线程 2 的主函数.
void *thmain2(void *arg);
// 线程 3 的主函数.
void *thmain3(void *arg);
// 线程 4 的主函数.
void *thmain4(void *arg);
// 线程 5 的主函数.
void *thmain5(void *arg);

int main() {
  // 线程 id, typedef unsigned long pthread_t.
  pthread_t thid1 = 0, thid2 = 0, thid3 = 0, thid4 = 0, thid5 = 0;

  // 创建线程.
  int *var1 = new int;
  *var1 = 1;
  if (pthread_create(&thid1, NULL, thmain1, (void *)var1) != 0) {
    printf("pthread_create() failed.\n");
    exit(-1);
  }

  int *var2 = new int;
  *var2 = 2;
  if (pthread_create(&thid2, NULL, thmain2, (void *)var2) != 0) {
    printf("pthread_create() failed.\n");
    exit(-1);
  }

  int *var3 = new int;
  *var3 = 3;
  if (pthread_create(&thid3, NULL, thmain3, (void *)var3) != 0) {
    printf("pthread_create() failed.\n");
    exit(-1);
  }

  int *var4 = new int;
  *var4 = 4;
  if (pthread_create(&thid4, NULL, thmain4, (void *)var4) != 0) {
    printf("pthread_create() failed.\n");
    exit(-1);
  }

  int *var5 = new int;
  *var5 = 5;
  if (pthread_create(&thid5, NULL, thmain5, (void *)var5) != 0) {
    printf("pthread_create() failed.\n");
    exit(-1);
  }

  // 等待子线程退出.
  printf("Join...\n");
  pthread_join(thid1, NULL);
  pthread_join(thid2, NULL);
  pthread_join(thid3, NULL);
  pthread_join(thid4, NULL);
  pthread_join(thid5, NULL);
  printf("Join ok.\n");

  return 0;
}

void *thmain1(void *arg) {
  printf("var1 = %d\n", *(int *)arg);
  delete (int *)arg;
  printf("Thread 1 begins to run.\n");

  pthread_exit(0);
}

void *thmain2(void *arg) {
  printf("var2 = %d\n", *(int *)arg);
  delete (int *)arg;
  printf("Thread 2 begins to run.\n");

  pthread_exit(0);
}

void *thmain3(void *arg) {
  printf("var3 = %d\n", *(int *)arg);
  delete (int *)arg;
  printf("Thread 3 begins to run.\n");

  pthread_exit(0);
}

void *thmain4(void *arg) {
  printf("var4 = %d\n", *(int *)arg);
  delete (int *)arg;
  printf("Thread 4 begins to run.\n");

  pthread_exit(0);
}

void *thmain5(void *arg) {
  printf("var5 = %d\n", *(int *)arg);
  delete (int *)arg;
  printf("Thread 5 begins to run.\n");

  pthread_exit(0);
}
